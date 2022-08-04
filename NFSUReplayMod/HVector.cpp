#include "stdafx.h"
#include "HVector.h"
#include "FromGame.h"

#define __free NFS_free
#define __malloc NFS_malloc
inline void* __realloc(void* old_ptr, size_t new_size) {
	if (old_ptr == 0) {
		return __malloc(new_size);
	}
	return NFS_realloc(old_ptr, new_size);
}

HVector::~HVector() {
	__free(_ptr);
}

size_t ToPowerOf2(size_t num) {
	DWORD bit;
	_BitScanReverse(&bit, num);
	size_t res = 1 << (bit + 1);
	return res;
}

void HVector::Expand(size_t new_size) {
	_capacity = ToPowerOf2(new_size);
	_ptr = (BYTE*)__realloc(_ptr, _capacity);
}

void HVector::resize(size_t new_size) {
	if (new_size > _capacity) {
		Expand(new_size);
	}
	_size_ = new_size;
}

void HVector::SetReaderOffset(size_t rdptr) {
#ifdef _DEBUG
	if (rdptr >= size())
		return;
#endif // _DEBUG

	_reader_offset = rdptr;
}

void HVector::PushBack(const void* data, size_t _size) {
	size_t cur_size = this->size();
	resize(cur_size + _size);
	R_memcpy(_ptr + cur_size, data, _size);
}

bool HVector::Read(void *data, size_t _size) {
	bool res = Read(data, _size, _reader_offset);
	if (res) {
		_reader_offset += _size;
	}
	return res;
}

bool HVector::Read(void *data, size_t _size, size_t offset) {
	if (offset + _size > size()) {
		return false;
	}
	R_memcpy(data, _ptr + offset, _size);
	return true;
}

void* HVector::VirtualRead(size_t _size) {
	void *res = GetMemPtr(_reader_offset);
#ifdef _DEBUG
	if (_reader_offset + _size > size()) {
		return nullptr;
	}
#endif
	_reader_offset += _size;
	return res;
}

void* HVector::ReservePlace(size_t _size) {
	size_t old_size = size();
	resize(old_size + _size);
	return GetMemPtr(old_size);
}
