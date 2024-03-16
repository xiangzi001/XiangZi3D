
struct Float3       //���λ����Ϣ�ͷ�����Ϣ����������
{
	float Data[3];     //x,y,z
};

struct Float2      //�������������������
{
	float Data[2];   //u,v
};

struct Face          //����Ϣ
{
	int vertex[3][3];       //�����㹹��һ����  ÿ����������������Ϣ
};

struct mMaterial
{
	float Ns;     //shinness
	float Ka[4];
	float Kd[4];
	float Ks[4];
	string mTextureName;
	mMaterial() {
		Ka[3] = 1.0;
		Kd[3] = 1.0;
		Ks[3] = 1.0;
	}
};

unsigned char* LoadFileContent(const char* path, int& filesize)   //�ú����������ȹȽ̳�
//��ȡ�ļ��������ļ����ݣ����ļ���С��ֵ��filesize       
{
	unsigned char* fileContent = nullptr;
	filesize = 0;
	FILE* pFile;
	fopen_s(&pFile, path, "rb");   //�����Ʒ�ʽ��ȡ
	if (pFile)
	{
		fseek(pFile, 0, SEEK_END);      //���ļ�ָ���ƶ����ļ�ĩβ
		int nLen = ftell(pFile);        //�����ļ�ͷ���ľ���   //����ָ�ļ���С
		if (nLen > 0)
		{
			rewind(pFile);          //�ƶ����ļ�ͷ��
			fileContent = new unsigned char[nLen + 1];
			//Ϊ�ļ�ָ�뿪�ٿռ�
			fread(fileContent, sizeof(unsigned char), nLen, pFile);
			//��pFile�����ݶ���fileContent
			fileContent[nLen] = '\0';         //�ļ�ĩβ����\0
			filesize = nLen;                  //Ϊ�ļ���С��ֵ
		}
		fclose(pFile);
	}
	return fileContent;
}

class objModel
{
public:
	void objLoad(const char* objFileName);
	void mtlLoad(const char* mtlFileName);

	string mtlName;
	string objName;

private:
	vector<Float3> mLocation;   //λ����Ϣ
	vector<Float3> mNormal;     //������Ϣ
	vector<Float2> mTexcoord;   //����������Ϣ
	vector<Face> mFace;         //����Ϣ
	vector<mMaterial> mMtl;		//Mtl��Ϣ
};

void objModel::objLoad(const char* objFileName)
{
	objName = objFileName;
	int nFileSize = 0;
	unsigned char* fileContent = LoadFileContent(objFileName, nFileSize);    //��ȡ�ļ�����
	if (fileContent == nullptr)     //�ļ�Ϊ��
	{
		return;
	}
	stringstream ssFileContent((char*)fileContent);   //����ȡ�ļ�����
	string temp;       //�����޹���Ϣ
	char szoneLine[256];        //��ȡһ�е�����
	while (!ssFileContent.eof())
	{
		memset(szoneLine, 0, 256);        //  ÿ��ѭ����ʼ������szoneLine
		ssFileContent.getline(szoneLine, 256);      //����ȡһ��
		if (strlen(szoneLine) > 0)       //���в�Ϊ��
		{
			if (szoneLine[0] == 'v')     //v��ͷ������
			{
				stringstream ssOneLine(szoneLine);        //���ݴ洢������ ���㸳ֵ
				if (szoneLine[1] == 't')       //������Ϣ
				{
					ssOneLine >> temp;     //���ܱ�ʶ�� vt
					Float2 tempTexcoord;
					ssOneLine >> tempTexcoord.Data[0] >> tempTexcoord.Data[1];   //���ݴ�����ʱ������
					mTexcoord.push_back(tempTexcoord);         //��������

				}
				if (szoneLine[1] == 'n')            //������Ϣ
				{
					ssOneLine >> temp;      //���ձ�ʶ��vn
					Float3 tempNormal;
					ssOneLine >> tempNormal.Data[0] >> tempNormal.Data[1] >> tempNormal.Data[2];
					mNormal.push_back(tempNormal);
				}
				else                          //���λ����Ϣ
				{
					ssOneLine >> temp;
					Float3 tempLocation;
					ssOneLine >> tempLocation.Data[0] >> tempLocation.Data[1] >> tempLocation.Data[2];
					mLocation.push_back(tempLocation);
				}
			}
			else if (szoneLine[0] == 'f')          //����Ϣ
			{
				stringstream ssOneLine(szoneLine);     //����ȡһ������
				ssOneLine >> temp; //���ձ�ʶ��f
				//    f��Ϣ    exp�� f 1/1/1 2/2/2 3/3/3      λ������/��������/��������   ������Ƭ �����㹹��һ����
				string vertexStr;   //������������
				Face tempFace;
				for (int i = 0; i < 3; ++i)         //ÿ����������
				{
					ssOneLine >> vertexStr;           //�����ж�ȡ���������Ϣ
					size_t pos = vertexStr.find_first_of('/');       //�ҵ���һ��/��λ��      //���ҵ����λ����Ϣ
					string locIndexStr = vertexStr.substr(0, pos);       //��ֵ���λ����Ϣ
					size_t pos2 = vertexStr.find_first_of('/', pos + 1);   //�ҵ��ڶ���/   ���ҵ��������������Ϣ
					string texIndexSrt = vertexStr.substr(pos + 1, pos2 - 1 - pos);       //��ֵ�������������Ϣ
					string norIndexSrt = vertexStr.substr(pos2 + 1, vertexStr.length() - 1 - pos2);   //��ֵ��ķ�����Ϣ
					tempFace.vertex[i][0] = atoi(locIndexStr.c_str());        //��������Ϣ�� srtingת��Ϊ int     //λ��������Ϣ��ֵ
					tempFace.vertex[i][1] = atoi(texIndexSrt.c_str());         //��������������Ϣ��ֵ
					tempFace.vertex[i][2] = atoi(norIndexSrt.c_str());         //������Ϣ��ֵ
				}
				mFace.push_back(tempFace);

				ssOneLine >> vertexStr;           //�����ж�ȡ���������Ϣ
				size_t pos = vertexStr.find_first_of('/');       //�ҵ���һ��/��λ��      //���ҵ����λ����Ϣ
				if (pos != string::npos) {
					string locIndexStr = vertexStr.substr(0, pos);       //��ֵ���λ����Ϣ
					size_t pos2 = vertexStr.find_first_of('/', pos + 1);   //�ҵ��ڶ���/   ���ҵ��������������Ϣ
					string texIndexSrt = vertexStr.substr(pos + 1, pos2 - 1 - pos);       //��ֵ�������������Ϣ
					string norIndexSrt = vertexStr.substr(pos2 + 1, vertexStr.length() - 1 - pos2);   //��ֵ��ķ�����Ϣ

					tempFace.vertex[1][0] = atoi(locIndexStr.c_str());        //��������Ϣ�� srtingת��Ϊ int     //λ��������Ϣ��ֵ
					tempFace.vertex[1][1] = atoi(texIndexSrt.c_str());         //��������������Ϣ��ֵ
					tempFace.vertex[1][2] = atoi(norIndexSrt.c_str());         //������Ϣ��ֵ
					mFace.push_back(tempFace);
				}
			}
		}   //end ��0��
	}  //end while
	delete fileContent;
}

void objModel::mtlLoad(const char* mtlFileName) {
	mtlName = mtlFileName;

	string temp;       //�����޹���Ϣ

	mMaterial tempMtl;
	
	fstream mtlFile;
	mtlFile.open(mtlFileName);

	while (!mtlFile.eof())
	{
		string temp;
		mtlFile >> temp;
		if (temp == "newmtl")
		{
			if (!tempMtl.mTextureName.empty()) {
				mMtl.push_back(tempMtl);
			}
			mtlFile >> tempMtl.mTextureName;
		}
		else if (temp == "Ns")
		{
			mtlFile >> tempMtl.Ns;
		}
		else if (temp == "Ka")
		{
			mtlFile >> tempMtl.Ka[0] >> tempMtl.Ka[1] >> tempMtl.Ka[2];
		}
		else if (temp == "Kd")
		{
			mtlFile >> tempMtl.Kd[0] >> tempMtl.Kd[1] >> tempMtl.Kd[2];
			/*
			MessageBox(_char(_str(tempMtl.Kd[0] * 100.0)));
			MessageBox(_char(_str(tempMtl.Kd[1] * 100.0)));
			MessageBox(_char(_str(tempMtl.Kd[2] * 100.0)));
			*/
		}
		else if (temp == "Ks")
		{
			mtlFile >> tempMtl.Ks[0] >> tempMtl.Ks[1] >> tempMtl.Ks[2];
		}
		else if (temp == "map_Kd")
		{
			mtlFile >> temp;
			size_t pos = temp.find_last_of('\\');
			temp = temp.substr(pos + 1, temp.length() - 1 - pos);
			//string tempF = "Res/" + temp;
			//const char* texFile = tempF.c_str();
			//tempMtl.mTextureId = CreateTexture(texFile);
			mMtl.push_back(tempMtl);
		}
	}

	if (!tempMtl.mTextureName.empty()) {
		mMtl.push_back(tempMtl);
	}
}

