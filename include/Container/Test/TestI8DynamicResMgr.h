#include "Container/I8DynamicResMgr.h"
#include <vector>
#include "Container/I8MemoyStream.h"

struct Info_st
{
    std::vector<TCHAR> clString;
    Info_st()
    {        
    }
    Info_st(LPCTSTR p)
    {
        clString.resize(lstrlen(p) + 1);
        memcpy(&clString[0], p, clString.size() * sizeof(TCHAR));
    }
    Info_st(TCHAR tChar, int iCount)
    {
        clString.resize(iCount + 1);
        for (int i = 0; i < iCount; ++i)
        {
            clString[i] = tChar;
        }
        clString[iCount] = 0;
    }
    bool operator==(const Info_st& stInfo) const
    {
        if (clString.size() != stInfo.clString.size())
        {
            return FALSE;
        }
        return memcmp(&clString[0], &stInfo.clString[0], clString.size() * sizeof(TCHAR)) == 0;
    }
    bool operator!=(const Info_st& stInfo) const
    {
        return !operator==(stInfo);
    }
};

CI8MemoyIStream& operator>>(CI8MemoyIStream& clStream, Info_st& stGameExInfo)
{
    DWORD dwCount;
    clStream>>dwCount;
    stGameExInfo.clString.resize(dwCount);
    clStream.Read(&stGameExInfo.clString[0], stGameExInfo.clString.size());
    return clStream;
}

CI8MemoyOStream& operator<<(CI8MemoyOStream& clStream, const Info_st& stGameExInfo)
{
    clStream<<DWORD(stGameExInfo.clString.size());
    clStream.Write(&stGameExInfo.clString[0], stGameExInfo.clString.size());
    return clStream;
}

void Check(CI8ResMgrFile<DWORD>& clFile, DWORD id, const Info_st& stInfo)
{
    char aiOut[1000];
    clFile.Get(id, aiOut, _countof(aiOut));
    CI8MemoyIStream clStream(aiOut, clFile.GetSize(id));
    Info_st stReadInfo;
    clStream>>stReadInfo;
    if (stReadInfo != stInfo)
    {
        assert(0);
    }
}

void Check(CI8DynamicResMgr<DWORD, Info_st>& clResMgr, DWORD id, const Info_st& stInfo)
{
    if (clResMgr.GetData(id) != stInfo)
    {
        assert(0);
    }
}


void TestCloseWithAutoSave()
{
    DeleteFile(_T("AAAA.dat"));
    CI8DynamicResMgr<DWORD, Info_st> clResMgr;
    if (clResMgr.SetFile(_T("AAAA.dat")) != 0)
    {
        assert(0);
    }
    clResMgr.SetData(100, _T("Good"));
    clResMgr.SetData(200, _T("Nick"));
    clResMgr.SetData(300, _T("Luck"));
    Check(clResMgr, 100, _T("Good"));
    Check(clResMgr, 200, _T("Nick"));
    Check(clResMgr, 300, _T("Luck"));
    Check(clResMgr, 300, _T("Luck"));
    Check(clResMgr, 100, _T("Good"));
    CI8ResMgrFile<DWORD> clFile;
    CopyFile(_T("AAAA.dat"), _T("Test.dat"), FALSE);
    if (clFile.Open(_T("Test.dat")) != 0)
    {
        assert(0);
    }
    if (clFile.GetCount() != 0)
    {
        assert(0);
    }
    if (clResMgr.GetCount() != 3)
    {
        assert(0);
    }
    clFile.Close();
    clResMgr.Clear();
    CopyFile(_T("AAAA.dat"), _T("Test.dat"), FALSE);
    if (clFile.Open(_T("Test.dat")) != 0)
    {
        assert(0);
    }
    Check(clFile, 100, _T("Good"));
    Check(clFile, 200, _T("Nick"));
    Check(clFile, 300, _T("Luck"));
    if (clFile.GetCount() != 3)
    {
        assert(0);
    }
    clFile.Close();
    if (clResMgr.SetFile(_T("AAAA.dat")) != 0)
    {
        assert(0);
    }
    Check(clResMgr, 100, _T("Good"));
    Check(clResMgr, 200, _T("Nick"));
    Check(clResMgr, 300, _T("Luck"));
    if (clResMgr.GetCount() != 3)
    {
        assert(0);
    }
    clResMgr.Clear();
}

void TestMinSize()
{
    DeleteFile(_T("AAAA.dat"));
    CI8DynamicResMgr<DWORD, Info_st> clResMgr;
    if (clResMgr.SetFile(_T("AAAA.dat")) != 0)
    {
        assert(0);
    }
    clResMgr.SetLimistSize(1);
    assert(clResMgr.GetActiveDataSize() == 0);
    clResMgr.SetData(100, _T("Good"));
    assert(clResMgr.GetActiveDataSize() == 14);
    clResMgr.SetData(200, _T("Nicks"));
    assert(clResMgr.GetActiveDataSize() == 16);
    clResMgr.SetData(300, _T("Do"));
    assert(clResMgr.GetActiveDataSize() == 10);
    clResMgr.Clear();
    if (clResMgr.SetFile(_T("AAAA.dat")) != 0)
    {
        assert(0);
    }
    assert(clResMgr.GetActiveDataSize() == 0);
    clResMgr.SetData(300, _T("FireWall"));
    assert(clResMgr.GetActiveDataSize() == 22);
    clResMgr.SetData(300, _T("Why"));
    assert(clResMgr.GetActiveDataSize() == 12);
    assert(clResMgr.GetCount() == 3);
    clResMgr.SetData(200, _T("123456"));
    assert(clResMgr.GetActiveDataSize() == 18);
    Check(clResMgr, 200, _T("123456"));
    assert(clResMgr.GetActiveDataSize() == 18);
    Check(clResMgr, 100, _T("Good"));
    assert(clResMgr.GetActiveDataSize() == 14);
}

void TestLimitSizeAfter()
{
    DeleteFile(_T("AAAA.dat"));
    CI8DynamicResMgr<DWORD, Info_st> clResMgr;
    if (clResMgr.SetFile(_T("AAAA.dat")) != 0)
    {
        assert(0);
    }
    clResMgr.SetData(10, _T("P"));//8
    clResMgr.SetData(100, _T("Good"));//14
    clResMgr.SetData(200, _T("Nicks"));//16
    clResMgr.SetData(300, _T("Do"));//10
    clResMgr.SetData(400, _T("For"));//12
    clResMgr.SetData(500, _T("WhileTT"));//20
    assert(clResMgr.GetActiveDataSize() == 80);
    clResMgr.SetLimistSize(40);
    assert(clResMgr.GetActiveDataSize() == 32);
    Check(clResMgr, 10, _T("P"));
    assert(clResMgr.GetActiveDataSize() == 40);
    Check(clResMgr, 10, _T("P"));
    assert(clResMgr.GetActiveDataSize() == 40);
    Check(clResMgr, 500, _T("WhileTT"));
    assert(clResMgr.GetActiveDataSize() == 40);
    Check(clResMgr, 10, _T("P"));
    assert(clResMgr.GetActiveDataSize() == 40);
    Check(clResMgr, 400, _T("For"));
    assert(clResMgr.GetActiveDataSize() == 40);
    Check(clResMgr, 100, _T("Good"));
    assert(clResMgr.GetActiveDataSize() == 34);
    clResMgr.DeleteData(200);
    assert(clResMgr.GetActiveDataSize() == 34);
    clResMgr.DeleteData(400);
    assert(clResMgr.GetActiveDataSize() == 22);
}

void TestSave()
{
    DeleteFile(_T("AAAA.dat"));
    CI8DynamicResMgr<DWORD, Info_st> clResMgr;
    if (clResMgr.SetFile(_T("AAAA.dat")) != 0)
    {
        assert(0);
    }
    clResMgr.SetData(10, _T("P"));//8
    clResMgr.SetData(100, _T("Good"));//14
    clResMgr.SetData(200, _T("Nicks"));//16
    clResMgr.SetData(300, _T("Do"));//10
    clResMgr.SetData(400, _T("For"));//12
    clResMgr.SetData(500, _T("WhileTT"));//20
    Check(clResMgr, 400, _T("For"));
    clResMgr.SetData(100, _T("Good Day"));
    clResMgr.DeleteData(400);
    Check(clResMgr, 100, _T("Good Day"));
    CI8ResMgrFile<DWORD> clFile;
    CopyFile(_T("AAAA.dat"), _T("Test.dat"), FALSE);
    if (clFile.Open(_T("Test.dat")) != 0)
    {
        assert(0);
    }
    if (clFile.GetCount() != 0)
    {
        assert(0);
    }
    clFile.Close();
    clResMgr.WriteFile();
    CopyFile(_T("AAAA.dat"), _T("Test.dat"), FALSE);
    if (clFile.Open(_T("Test.dat")) != 0)
    {
        assert(0);
    }
    if (clFile.GetCount() != 5)
    {
        assert(0);
    }
    Check(clFile, 100, _T("Good Day"));
    clFile.Close();
    clResMgr.DeleteData(10);
    clResMgr.SetData(10, _T("tttt"));
    clResMgr.DeleteData(10);
    clResMgr.WriteFile();
    CopyFile(_T("AAAA.dat"), _T("Test.dat"), FALSE);
    if (clFile.Open(_T("Test.dat")) != 0)
    {
        assert(0);
    }
    if (clFile.GetCount() != 4 || clFile.IsExist(10))
    {
        assert(0);
    }
}

void TestSaveForLimitSize()
{
    DeleteFile(_T("AAAA.dat"));
    CI8DynamicResMgr<DWORD, Info_st> clResMgr;
    if (clResMgr.SetFile(_T("AAAA.dat")) != 0)
    {
        assert(0);
    }
    clResMgr.SetLimistSize(50);
    clResMgr.SetData(10, _T("P"));//8
    clResMgr.SetData(100, _T("Good"));//14
    clResMgr.SetData(200, _T("Nicks"));//16
    clResMgr.SetData(300, _T("Do"));//10
    CI8ResMgrFile<DWORD> clFile;
    CopyFile(_T("AAAA.dat"), _T("Test.dat"), FALSE);
    if (clFile.Open(_T("Test.dat")) != 0)
    {
        assert(0);
    }
    if (clFile.GetCount() != 0)
    {
        assert(0);
    }
    clFile.Close();
    clResMgr.SetData(200, _T("Nicks!!!"));//16
    CopyFile(_T("AAAA.dat"), _T("Test.dat"), FALSE);
    if (clFile.Open(_T("Test.dat")) != 0)
    {
        assert(0);
    }
    if (clFile.GetCount() != 4)
    {
        assert(0);
    }
    clFile.Close();
    clResMgr.SetData(10, _T("X"));//8
    clResMgr.DeleteData(300);
    CopyFile(_T("AAAA.dat"), _T("Test.dat"), FALSE);
    if (clFile.Open(_T("Test.dat")) != 0)
    {
        assert(0);
    }
    if (clFile.GetCount() != 4)
    {
        assert(0);
    }
    clFile.Close();
    clResMgr.WriteFile();
    CopyFile(_T("AAAA.dat"), _T("Test.dat"), FALSE);
    if (clFile.Open(_T("Test.dat")) != 0)
    {
        assert(0);
    }
    if (clFile.GetCount() != 3)
    {
        assert(0);
    }
    Check(clFile, 10, _T("X"));
    clFile.Close();
    clResMgr.DeleteData(10);
    CopyFile(_T("AAAA.dat"), _T("Test.dat"), FALSE);
    if (clFile.Open(_T("Test.dat")) != 0)
    {
        assert(0);
    }
    if (clFile.GetCount() != 3)
    {
        assert(0);
    }
    clFile.Close();
    clResMgr.WriteFile();
    CopyFile(_T("AAAA.dat"), _T("Test.dat"), FALSE);
    if (clFile.Open(_T("Test.dat")) != 0)
    {
        assert(0);
    }
    if (clFile.GetCount() != 2)
    {
        assert(0);
    }
}


void TestAutoDefrag()
{
    DeleteFile(_T("AAAA.dat"));
    CI8DynamicResMgr<DWORD, Info_st> clResMgr;
    if (clResMgr.SetFile(_T("AAAA.dat")) != 0)
    {
        assert(0);
    }
    clResMgr.SetData(100, Info_st('A', 10330));
    clResMgr.SetData(200, Info_st('B', 12200));
    clResMgr.SetData(233, Info_st('G', 50));
    clResMgr.WriteFile();
    clResMgr.SetData(100, Info_st('C', 11330));
    clResMgr.SetData(200, Info_st('D', 13200));
    clResMgr.WriteFile();
    CopyFile(_T("AAAA.dat"), _T("Test.dat"), FALSE);
    CI8ResMgrFile<DWORD> clFile;
    if (clFile.Open(_T("Test.dat")) != 0)
    {
        assert(0);
    }
    INT64 lFileSizeA = clFile.GetFileSize();
    INT64 lUsedSizeA = clFile.GetUsedSize();
    assert(lFileSizeA - lUsedSizeA == (10330 + 12200) * 2 + 4 * 3);
    clFile.Close();
    clResMgr.SetData(300, Info_st('E', 13250));
    clResMgr.DeleteData(100);
    clResMgr.WriteFile();
    CopyFile(_T("AAAA.dat"), _T("Test.dat"), FALSE);
    if (clFile.Open(_T("Test.dat")) != 0)
    {
        assert(0);
    }
    INT64 lFileSizeB = clFile.GetFileSize();
    INT64 lUsedSizeB = clFile.GetUsedSize();
    assert(lFileSizeB == lUsedSizeB);
    assert(lUsedSizeB > lUsedSizeA);
    assert(lFileSizeB < lFileSizeA);
    clFile.Close();
}

void TestUpdateTime()
{
    DeleteFile(_T("AAAA.dat"));
    CI8DynamicResMgr<DWORD, Info_st> clResMgr;
    if (clResMgr.SetFile(_T("AAAA.dat")) != 0)
    {
        assert(0);
    }
    clResMgr.SetData(100, Info_st('A', 10330));
    clResMgr.SetData(200, Info_st('B', 12200));
    clResMgr.SetData(233, Info_st('G', 50));
    clResMgr.SetData(100, Info_st('C', 11330));
    clResMgr.SetData(200, Info_st('D', 13200));
    clResMgr.Clear();
    if (clResMgr.SetFile(_T("AAAA.dat")) != 0)
    {
        assert(0);
    }
    clResMgr.Clear();
    SYSTEMTIME stSystemTime;
    GetSystemTime(&stSystemTime);
    stSystemTime.wHour = (stSystemTime.wHour + 1) % 24;
    FILETIME stFileTime;
    SystemTimeToFileTime(&stSystemTime, &stFileTime);
    HANDLE hFile = CreateFile(_T("AAAA.dat"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    SetFileTime(hFile, NULL, NULL, &stFileTime);
    CloseHandle(hFile);
    if (clResMgr.SetFile(_T("AAAA.dat")) != CI8ResMgrFile<DWORD>::E_RST_ERR_UPDATETIME)
    {
        assert(0);
    }
}

void TestOnlyRead()
{
    DeleteFile(_T("AAAA.dat"));
    CI8DynamicResMgr<DWORD, Info_st> clReadResMgr;
    if (clReadResMgr.SetFile(_T("AAAA.dat")) != 0)
    {
        assert(0);
    }
    CI8DynamicResMgr<DWORD, Info_st> clWriteResMgr;
    if (clWriteResMgr.SetFile(_T("AAAA.dat")) != 0)
    {
        assert(0);
    }
    clWriteResMgr.SetData(100, Info_st('A', 10330));
    clWriteResMgr.SetData(200, Info_st('B', 12200));
    clWriteResMgr.SetData(233, Info_st('G', 50));
    clReadResMgr.Clear();
    clWriteResMgr.Clear();
    if (clWriteResMgr.SetFile(_T("AAAA.dat")) != 0)
    {
        assert(0);
    }
    Check(clWriteResMgr, 100, Info_st('A', 10330));
    Check(clWriteResMgr, 200, Info_st('B', 12200));
    Check(clWriteResMgr, 233, Info_st('G', 50));
}

void TestGetInexistData()
{
    DeleteFile(_T("AAAA.dat"));
    CI8DynamicResMgr<DWORD, Info_st> clResMgr;
    if (clResMgr.SetFile(_T("AAAA.dat")) != 0)
    {
        assert(0);
    }
    const Info_st* pstInfo = clResMgr.Find(100);
    assert(pstInfo == NULL);
}

void TestCreateFileInInexistDirectory()
{
    system("rd /s/q AA");
    CI8DynamicResMgr<DWORD, Info_st> clResMgr;
    if (clResMgr.SetFile(_T("AA\\BB\\CC\\AAAA.dat")) != 0)
    {
        assert(0);
    }
    clResMgr.SetData(100, Info_st('A', 10330));
    clResMgr.SetData(200, Info_st('B', 12200));
    clResMgr.Clear();
    if (clResMgr.SetFile(_T("AA\\BB\\CC\\AAAA.dat")) != 0)
    {
        assert(0);
    }
    Check(clResMgr, 100, Info_st('A', 10330));
    Check(clResMgr, 200, Info_st('B', 12200));
}

void TestI8DynamicResMgr()
{
    TestCloseWithAutoSave();
    TestMinSize();
    TestLimitSizeAfter();
    TestSave();
    TestSaveForLimitSize();
    TestAutoDefrag();
    TestUpdateTime();
    TestOnlyRead();
    TestGetInexistData();
    TestCreateFileInInexistDirectory();
}


