
size_t lengthString(string* str) {
	return sizeof(str) / sizeof(str[0]);
}

size_t posList(string str_1, string** str_2, size_t len) {
//	size_t len= lengthString(str_2);
	for (size_t i = 0; i < len; i++) {
		if (str_1 == (*(str_2[i]))  )
			return i;
	}
	return -1;
}
size_t posList(string str_1, char** str_2, size_t len) {
	//	size_t len= lengthString(str_2);
	for (size_t i = 0; i < len; i++) {
		if (str_1 == str_2[i])
			return i;
	}
	return -1;
}

int Small(int a, int b) {
	if (a > b) {
		return b;
	}
	else {
		return a;
	}
}

class List_ {
	int num;
	int len;
	void** * data;

	List_()
		: num(0), len(0), data(0)
	{
	}
	~List_() {
		for (int i = 0; i < num; i++) {
			for (int j = 0; j < len; j++) {
				delete data[i][j];
			}
			delete[] data[i];
		}
		delete[] data;
	}

	void set_num(int n) {
		void *** _data = new void** [n];
		int s = Small(num, n);
		for (int i = 0; i < s; i++) {
			_data[i] = data[i];
		}
		for (int i = s; i < n; i++) {
			_data[i] = new void* [len];
		}
		for (int i = s; i < num; i++) {
			for (int j = 0; j < len; j++) {
				delete data[i][j];
			}
			delete[] data[i];
		}
		delete[] data;
		num = n;
		data = _data;
	}

	void set_len(int l) {
		for (int i = 0; i < num; i++) {
			for (int j = 0; j < len; j++) {
				delete data[i][j];
			}
			delete[] data[i];
			data[i] = new void* [len];
		}
	}

	void set(int l, ...) {
		va_list temp;
		va_start(temp, l);

		for (int i = 0; i < num; i++) {
			data[i][l] = va_arg(temp, void*);
		}

		va_end(temp);
	}

	void* get(int n, int l) {
		return data[n][l];
	}
};

template<class T>
class List {
public:
	int len;
	T** data;

	List()
		: len(0), data(0)
	{
	}
	~List() {
		for (int j = 0; j < len; j++) {
			delete data[j];
		}
		delete[] data;
	}

	void destroy(int l) {
		delete data[l];
	}

	void destroy() {
		for (int j = 0; j < len; j++) {
			delete data[j];
		}
		delete[] data;
	}

	void change(int l) {
		T** _data = new T* [l];
		int s = Small(len, l);
		for (int i = 0; i < s; i++) {
			_data[i] = data[i];
		}
		for (int i = s; i < l; i++) {
			_data[i] = new T;
		}
		for (int i = s; i < len; i++) {
			delete data[i];
		}
		delete[] data;
		len = l;
		data = _data;
	}

	void set(int l, T* n) {
		delete data[l];
		data[l] = n;
	}

	T* get(int l) {
		return data[l];
	}

	T* operator [](int l) {
		return data[l];
	}
};