#include "Container/I8ResMgrFile.h"

template<int iCount>
void Set(CI8ResMgrFile<DWORD>& clFile, DWORD id, const int(&Data)[iCount], BOOL bSaveIndex = TRUE)
{
    clFile.Set(id, Data, sizeof(DWORD) * _countof(Data), bSaveIndex);
}

template<int iCount>
void Check(CI8ResMgrFile<DWORD>& clFile, DWORD id, const int(&Data)[iCount])
{
    int aiOut[1000];
    clFile.Get(id, aiOut, _countof(aiOut));
    if (memcmp(aiOut, Data, max(sizeof(DWORD) * _countof(Data), clFile.GetSize(id))) != 0)
    {
        assert(0);
    }
}

void TestCreateNullAndOpenNull()
{
    DeleteFile(_T("Test.dat"));
    CI8ResMgrFile<DWORD> clFile;
    if (clFile.Open(_T("Test.dat")) != 0)
    {
        assert(0);
    }
    clFile.Close();
    if (clFile.Open(_T("Test.dat")) != 0)
    {
        assert(0);
    }
    clFile.Close();
}

void TestAddSetOpen()
{
    DeleteFile(_T("Test.dat"));
    CI8ResMgrFile<DWORD> clFile;
    if (clFile.Open(_T("Test.dat")) != 0)
    {
        assert(0);
    }
    int aiDataA[] = {1, 2, 3, 4, 5};
    int aiDataB[] = {2, 4, 6, 8, 10, 12, 14};
    int aiDataC[] = {11, 22, 33};
    int aiDataD[] = {9, 8};
    int aiDataE[] = {10, 20, 30, 40, 50, 60, 70, 80};
    Set(clFile, 50, aiDataA);
    Set(clFile, 70, aiDataB);
    Set(clFile, 20, aiDataC);

    Check(clFile, 50, aiDataA);
    Check(clFile, 70, aiDataB);
    Check(clFile, 20, aiDataC);

    Set(clFile, 50, aiDataD);

    Check(clFile, 50, aiDataD);
    Check(clFile, 70, aiDataB);
    Check(clFile, 20, aiDataC);

    Set(clFile, 50, aiDataE);

    Check(clFile, 50, aiDataE);
    Check(clFile, 70, aiDataB);
    Check(clFile, 20, aiDataC);
    if (clFile.GetCount() != 3)
    {
        assert(0);
    }

    clFile.Close();

    if (clFile.Open(_T("Test.dat")) != 0)
    {
        assert(0);
    }

    Check(clFile, 50, aiDataE);
    Check(clFile, 70, aiDataB);
    Check(clFile, 20, aiDataC);
    if (clFile.GetCount() != 3)
    {
        assert(0);
    }

    clFile.Close();
}
void TestRecycle()
{
    DeleteFile(_T("Test.dat"));
    CI8ResMgrFile<DWORD> clFile;
    if (clFile.Open(_T("Test.dat")) != 0)
    {
        assert(0);
    }
    int aiDataA[] = {1, 2, 3, 4, 5};
    int aiDataB[] = {2, 4, 6, 8, 10, 12, 14};
    int aiDataC[] = {11, 22, 33};
    int aiDataD[] = {9, 8};
    int aiDataE[] = {10, 20, 30, 40, 50, 60, 70, 80};
    Set(clFile, 50, aiDataE);
    Set(clFile, 70, aiDataA);
    Set(clFile, 20, aiDataB);

    INT64 lFileSizeA = clFile.GetFileSize();

    clFile.Del(50);
    clFile.Del(70);

    INT64 lFileSizeB = clFile.GetFileSize();

    Set(clFile, 50, aiDataC);
    Set(clFile, 70, aiDataB);

    INT64 lFileSizeC = clFile.GetFileSize();
    if (lFileSizeA != lFileSizeB || lFileSizeC != lFileSizeB)
    {
        assert(0);
    }

    Set(clFile, 20, aiDataE);

    Check(clFile, 50, aiDataC);
    Check(clFile, 70, aiDataB);
    Check(clFile, 20, aiDataE);
}

void TestRecycleAll()
{
    DeleteFile(_T("Test.dat"));
    CI8ResMgrFile<DWORD> clFile;
    if (clFile.Open(_T("Test.dat")) != 0)
    {
        assert(0);
    }
    int aiDataA[] = {1};
    int aiDataB[] = {2, 4};
    int aiDataC[] = {3, 6, 9};
    int aiDataD[] = {4, 8, 12, 16};
    int aiDataE[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    clFile.WriteIndex();
    Set(clFile, 10, aiDataA);
    Set(clFile, 20, aiDataB);
    Set(clFile, 30, aiDataC);
    Set(clFile, 40, aiDataD);

    INT64 lFileSizeA = clFile.GetFileSize();

    clFile.Del(10);
    clFile.Del(20);
    clFile.Del(30);
    Set(clFile, 40, aiDataE);

    INT64 lFileSizeB = clFile.GetFileSize();
    if (lFileSizeA != lFileSizeB)
    {
        assert(0);
    }
    Check(clFile, 40, aiDataE);
}


void TestAutoSaveIndex()
{
    DeleteFile(_T("Test.dat"));
    CI8ResMgrFile<DWORD> clFile;
    if (clFile.Open(_T("Test.dat")) != 0)
    {
        assert(0);
    }
    int aiDataA[] = {1};
    int aiDataB[] = {2, 4};
    int aiDataC[] = {3, 6, 9};
    int aiDataD[] = {4, 8, 12, 16};
    int aiDataE[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    Set(clFile, 10, aiDataA, FALSE);
    Set(clFile, 20, aiDataB, FALSE);
    Set(clFile, 30, aiDataC, FALSE);
    Set(clFile, 40, aiDataD, FALSE);
    Set(clFile, 30, aiDataE, FALSE);
    Set(clFile, 20, aiDataA, FALSE);
    clFile.Close();
    if (clFile.Open(_T("Test.dat")) != 0)
    {
        assert(0);
    }
    Check(clFile, 10, aiDataA);
    Check(clFile, 40, aiDataD);
    Check(clFile, 30, aiDataE);
    Check(clFile, 20, aiDataA);
}

void TestDefrag()
{
    DeleteFile(_T("Test.dat"));
    CI8ResMgrFile<DWORD> clFile;
    if (clFile.Open(_T("Test.dat")) != 0)
    {
        assert(0);
    }
    int aiDataA[] = {1};
    int aiDataB[] = {2, 4};
    int aiDataC[] = {3, 6, 9};
    int aiDataD[] = {4, 8, 12, 16};
    int aiDataE[] = {7, 21, 28, 35, 42, 49};
    int aiDataF[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int aiDataG[] = {1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25};
    int aiDataH[] = {11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31, 33, 35, 37, 39, 41};
    Set(clFile, 50, aiDataA, FALSE);
    Set(clFile, 10, aiDataE, FALSE);
    clFile.WriteIndex();
    INT64 lFileSize = clFile.GetFileSize();
    Set(clFile, 20, aiDataF, FALSE);
    lFileSize = clFile.GetFileSize();
    Set(clFile, 30, aiDataG, FALSE);
    lFileSize = clFile.GetFileSize();
    Set(clFile, 40, aiDataH, FALSE);
    lFileSize = clFile.GetFileSize();
    Set(clFile, 40, aiDataD, FALSE);
    Set(clFile, 30, aiDataC, FALSE);
    Set(clFile, 20, aiDataB, FALSE);
    Set(clFile, 10, aiDataA, FALSE);
    clFile.Del(50, FALSE);
    clFile.WriteIndex();
    INT64 lFileSizeA = clFile.GetFileSize();
    assert(lFileSizeA == 40 + 1024 + (6 + 10 + 13 + 16 + 1) * 4);
    clFile.Defrag();
    clFile.Close();
    if (clFile.Open(_T("Test.dat")) != 0)
    {
        assert(0);
    }
    Check(clFile, 10, aiDataA);
    Check(clFile, 20, aiDataB);
    Check(clFile, 30, aiDataC);
    Check(clFile, 40, aiDataD);
    assert(clFile.GetCount() == 4);
    INT64 lFileSizeB = clFile.GetFileSize();
    assert(lFileSizeB == 40 + 1024 + (1 + 2 + 3 + 4) * 4);
    Set(clFile, 50, aiDataE);
    Set(clFile, 60, aiDataF);
    Set(clFile, 50, aiDataG);
    Set(clFile, 60, aiDataH);
    INT64 lFileSizeC = clFile.GetFileSize();
    assert(lFileSizeC == 40 + 1024 + (1 + 2 + 3 + 4 + 6 + 10 + 13 + 16) * 4);
    clFile.Defrag();
    clFile.Close();
    if (clFile.Open(_T("Test.dat")) != 0)
    {
        assert(0);
    }
    Check(clFile, 10, aiDataA);
    Check(clFile, 20, aiDataB);
    Check(clFile, 30, aiDataC);
    Check(clFile, 40, aiDataD);
    Check(clFile, 50, aiDataG);
    Check(clFile, 60, aiDataH);
    assert(clFile.GetCount() == 6);
    INT64 lFileSizeD = clFile.GetFileSize();
    assert(lFileSizeD == 40 + 1024 + (1 + 2 + 3 + 4 + 13 + 16) * 4);
    clFile.Del(60);
    INT64 lFileSizeE = clFile.GetFileSize();
    assert(lFileSizeE == 40 + 1024 + (1 + 2 + 3 + 4 + 13) * 4);
}
void TestI8ResMgrFile()
{
    TestCreateNullAndOpenNull();
    TestAddSetOpen();
    TestRecycle();
    TestRecycleAll();
    TestAutoSaveIndex();
    TestDefrag();
}


