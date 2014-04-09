#include <stdio.h>
#include <ObjIdl.h>

#pragma once

class FileStream : public IStream
{
private:
	LONG _refcount;
	HANDLE _fh;
public:
	FileStream(HANDLE fh)
	{
		_refcount = 1;
		_fh = fh;
	}

	virtual HRESULT STDMETHODCALLTYPE Read(void *pv, ULONG cb, ULONG *pcbRead)
	{
		BOOL rc = ReadFile(_fh, pv, cb, pcbRead, NULL);
		return rc ? S_OK : HRESULT_FROM_WIN32(GetLastError());
	}

	virtual HRESULT STDMETHODCALLTYPE Write(const void *pv, ULONG cb, ULONG *pcbWritten)
	{
		BOOL rc = WriteFile(_fh, pv, cb, pcbWritten, NULL);
		return rc ? S_OK : HRESULT_FROM_WIN32(GetLastError());
	}

	virtual HRESULT STDMETHODCALLTYPE Seek(LARGE_INTEGER dist, DWORD origin, ULARGE_INTEGER *ptr)
	{
		DWORD dwMoveMethod;

		switch(origin)
		{
		case STREAM_SEEK_SET:
			dwMoveMethod = FILE_BEGIN;
			break;
		case STREAM_SEEK_CUR:
			dwMoveMethod = FILE_CURRENT;
			break;
		case STREAM_SEEK_END:
			dwMoveMethod = FILE_END;
		default:
			return STG_E_INVALIDFUNCTION;
		}

		if(SetFilePointerEx(_fh, dist, (PLARGE_INTEGER)ptr, dwMoveMethod) == 0)
			return HRESULT_FROM_WIN32(GetLastError());
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE Stat(STATSTG *pStatstg, DWORD)
	{
		if(GetFileSizeEx(_fh, (PLARGE_INTEGER)&pStatstg->cbSize) == 0)
			return HRESULT_FROM_WIN32(GetLastError());
		return S_OK;
	}

	virtual ULONG STDMETHODCALLTYPE AddRef(void)
	{
		return (ULONG)InterlockedIncrement(&_refcount);
	}

	virtual ULONG STDMETHODCALLTYPE Release(void)
	{
		ULONG res = (ULONG)InterlockedDecrement(&_refcount);
		if(res == 0) delete this;
		return res;
	}

	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void **ppvObject)
	{
		if(iid == __uuidof(IUnknown)
			|| iid == __uuidof(IStream)
			|| iid == __uuidof(ISequentialStream))
		{
			*ppvObject = static_cast<IStream*>(this);
			AddRef();
			return S_OK;
		}
		else return E_NOINTERFACE; 
	}

	virtual HRESULT STDMETHODCALLTYPE SetSize(ULARGE_INTEGER)
	{
		return E_NOTIMPL;
	}
	
	virtual HRESULT STDMETHODCALLTYPE CopyTo(IStream*, ULARGE_INTEGER, ULARGE_INTEGER *, ULARGE_INTEGER *)
	{
		return E_NOTIMPL;
	}
	
	virtual HRESULT STDMETHODCALLTYPE Commit(DWORD)
	{
		return E_NOTIMPL;
	}
	
	virtual HRESULT STDMETHODCALLTYPE Revert(void)
	{
		return E_NOTIMPL;
	}
	
	virtual HRESULT STDMETHODCALLTYPE LockRegion(ULARGE_INTEGER, ULARGE_INTEGER, DWORD)
	{
		return E_NOTIMPL;
	}
	
	virtual HRESULT STDMETHODCALLTYPE UnlockRegion(ULARGE_INTEGER, ULARGE_INTEGER, DWORD)
	{
		return E_NOTIMPL;
	}
	
	virtual HRESULT STDMETHODCALLTYPE Clone(IStream **)
	{
		return E_NOTIMPL;
	}
};