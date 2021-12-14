
class HVector {
private:
	
	BYTE* _ptr;
	size_t _size_;
	size_t _capacity;
	size_t _reader_offset;

	void Expand(size_t new_size);

public:
	inline HVector() {
		_ptr = 0;
		_size_ = 0;
		_capacity = 0;
		_reader_offset = 0;
	}
	~HVector();

	void PushBack(const void* data, size_t _size);

	bool Read(void *data, size_t _size);
	bool Read(void *data, size_t _size, size_t ptr);
	void* VirtualRead(size_t _size);

	void SetReaderOffset(size_t rdptr);

	inline size_t GetReaderOffset() {
		return _reader_offset;
	}

	inline size_t size() {
		return _size_;
	}
	void resize(size_t new_size);

	inline void* GetMemPtr(size_t offset) {
		return _ptr + offset;
	}

	size_t capacity() {
		return _capacity;
	}

	void* ReservePlace(size_t _size);
};