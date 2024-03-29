
struct Float3       //点的位置信息和法线信息的数据类型
{
	float Data[3];     //x,y,z
};

struct Float2      //点的纹理坐标数据类型
{
	float Data[2];   //u,v
};

struct Face          //面信息
{
	int vertex[3][3];       //三个点构成一个面  每个点有三个索引信息
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

unsigned char* LoadFileContent(const char* path, int& filesize)   //该函数来自于萌谷教程
//读取文件，返回文件内容，把文件大小赋值给filesize       
{
	unsigned char* fileContent = nullptr;
	filesize = 0;
	FILE* pFile;
	fopen_s(&pFile, path, "rb");   //二进制方式读取
	if (pFile)
	{
		fseek(pFile, 0, SEEK_END);      //将文件指针移动到文件末尾
		int nLen = ftell(pFile);        //距离文件头部的距离   //这里指文件大小
		if (nLen > 0)
		{
			rewind(pFile);          //移动到文件头部
			fileContent = new unsigned char[nLen + 1];
			//为文件指针开辟空间
			fread(fileContent, sizeof(unsigned char), nLen, pFile);
			//将pFile的内容读入fileContent
			fileContent[nLen] = '\0';         //文件末尾加上\0
			filesize = nLen;                  //为文件大小赋值
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
	vector<Float3> mLocation;   //位置信息
	vector<Float3> mNormal;     //法线信息
	vector<Float2> mTexcoord;   //纹理坐标信息
	vector<Face> mFace;         //面信息
	vector<mMaterial> mMtl;		//Mtl信息
};

void objModel::objLoad(const char* objFileName)
{
	objName = objFileName;
	int nFileSize = 0;
	unsigned char* fileContent = LoadFileContent(objFileName, nFileSize);    //读取文件内容
	if (fileContent == nullptr)     //文件为空
	{
		return;
	}
	stringstream ssFileContent((char*)fileContent);   //流读取文件内容
	string temp;       //接受无关信息
	char szoneLine[256];        //读取一行的数据
	while (!ssFileContent.eof())
	{
		memset(szoneLine, 0, 256);        //  每次循环初始化数组szoneLine
		ssFileContent.getline(szoneLine, 256);      //流读取一行
		if (strlen(szoneLine) > 0)       //该行不为空
		{
			if (szoneLine[0] == 'v')     //v开头的数据
			{
				stringstream ssOneLine(szoneLine);        //数据存储到流中 方便赋值
				if (szoneLine[1] == 't')       //纹理信息
				{
					ssOneLine >> temp;     //接受标识符 vt
					Float2 tempTexcoord;
					ssOneLine >> tempTexcoord.Data[0] >> tempTexcoord.Data[1];   //数据存入临时变量中
					mTexcoord.push_back(tempTexcoord);         //存入容器

				}
				if (szoneLine[1] == 'n')            //法线信息
				{
					ssOneLine >> temp;      //接收标识符vn
					Float3 tempNormal;
					ssOneLine >> tempNormal.Data[0] >> tempNormal.Data[1] >> tempNormal.Data[2];
					mNormal.push_back(tempNormal);
				}
				else                          //点的位置信息
				{
					ssOneLine >> temp;
					Float3 tempLocation;
					ssOneLine >> tempLocation.Data[0] >> tempLocation.Data[1] >> tempLocation.Data[2];
					mLocation.push_back(tempLocation);
				}
			}
			else if (szoneLine[0] == 'f')          //面信息
			{
				stringstream ssOneLine(szoneLine);     //流读取一行数据
				ssOneLine >> temp; //接收标识符f
				//    f信息    exp： f 1/1/1 2/2/2 3/3/3      位置索引/纹理索引/法线索引   三角面片 三个点构成一个面
				string vertexStr;   //接收流的内容
				Face tempFace;
				for (int i = 0; i < 3; ++i)         //每个面三个点
				{
					ssOneLine >> vertexStr;           //从流中读取点的索引信息
					size_t pos = vertexStr.find_first_of('/');       //找到第一个/的位置      //即找到点的位置信息
					string locIndexStr = vertexStr.substr(0, pos);       //赋值点的位置信息
					size_t pos2 = vertexStr.find_first_of('/', pos + 1);   //找到第二个/   即找到点的纹理坐标信息
					string texIndexSrt = vertexStr.substr(pos + 1, pos2 - 1 - pos);       //赋值点的纹理坐标信息
					string norIndexSrt = vertexStr.substr(pos2 + 1, vertexStr.length() - 1 - pos2);   //赋值点的法线信息
					tempFace.vertex[i][0] = atoi(locIndexStr.c_str());        //将索引信息从 srting转换为 int     //位置索引信息赋值
					tempFace.vertex[i][1] = atoi(texIndexSrt.c_str());         //纹理坐标索引信息赋值
					tempFace.vertex[i][2] = atoi(norIndexSrt.c_str());         //法线信息赋值
				}
				mFace.push_back(tempFace);

				ssOneLine >> vertexStr;           //从流中读取点的索引信息
				size_t pos = vertexStr.find_first_of('/');       //找到第一个/的位置      //即找到点的位置信息
				if (pos != string::npos) {
					string locIndexStr = vertexStr.substr(0, pos);       //赋值点的位置信息
					size_t pos2 = vertexStr.find_first_of('/', pos + 1);   //找到第二个/   即找到点的纹理坐标信息
					string texIndexSrt = vertexStr.substr(pos + 1, pos2 - 1 - pos);       //赋值点的纹理坐标信息
					string norIndexSrt = vertexStr.substr(pos2 + 1, vertexStr.length() - 1 - pos2);   //赋值点的法线信息

					tempFace.vertex[1][0] = atoi(locIndexStr.c_str());        //将索引信息从 srting转换为 int     //位置索引信息赋值
					tempFace.vertex[1][1] = atoi(texIndexSrt.c_str());         //纹理坐标索引信息赋值
					tempFace.vertex[1][2] = atoi(norIndexSrt.c_str());         //法线信息赋值
					mFace.push_back(tempFace);
				}
			}
		}   //end 非0行
	}  //end while
	delete fileContent;
}

void objModel::mtlLoad(const char* mtlFileName) {
	mtlName = mtlFileName;

	string temp;       //接受无关信息

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

