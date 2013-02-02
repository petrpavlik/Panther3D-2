#include "precompiled.h"

#include "IRenderer.h"
#include "ISound.h"
#include "IEngine.h"
#include "IPhysEngine.h"


#include "symbols.h"
#include "profiler.h"
#include "types.h" // only for test purposes


#include <dbghelp.h>
#include <shellapi.h>
#include <shlobj.h>

#include "json/json.h"
#include "thread.h"


/** TEST: ICU */
// #include "unicode/utypes.h"   /* Basic ICU data types */
// #include "unicode/ucnv.h"     /* C   Converter API    */
// #include "unicode/ustring.h"  /* some more string fcns*/
// #include "unicode/uchar.h"    /* char names           */
// #include "unicode/uloc.h"
// #include "unicode/unistr.h"
// #include "unicode/translit.h" /* transliteration */
/** /TEST: ICU */




#if !defined(_DEBUG)
int GenerateDump(EXCEPTION_POINTERS* pExceptionPointers)
{
	BOOL bMiniDumpSuccessful;
	wchar szPath[MAX_PATH]; 
	wchar szFileName[MAX_PATH]; 
	wchar* szAppName = _W("P3D2TestApp");
	wchar* szVersion = _W("v0.1");
	DWORD dwBufferSize = MAX_PATH;
	HANDLE hDumpFile;
	SYSTEMTIME stLocalTime;
	MINIDUMP_EXCEPTION_INFORMATION ExpParam;

	GetLocalTime( &stLocalTime );
	GetCurrentDirectory( dwBufferSize, szPath );

	wsprintf( szFileName, MAX_PATH-1, _W("%s\\%s-%04d%02d%02d-%02d%02d%02d-%ld-%ld.dmp"), 
		szPath, szAppName, szVersion, 
		stLocalTime.wYear, stLocalTime.wMonth, stLocalTime.wDay, 
		stLocalTime.wHour, stLocalTime.wMinute, stLocalTime.wSecond, 
		GetCurrentProcessId(), GetCurrentThreadId());
	hDumpFile = CreateFile(szFileName, GENERIC_READ|GENERIC_WRITE, 
		FILE_SHARE_WRITE|FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);

	ExpParam.ThreadId = GetCurrentThreadId();
	ExpParam.ExceptionPointers = pExceptionPointers;
	ExpParam.ClientPointers = TRUE;

	bMiniDumpSuccessful = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), 
		hDumpFile, MiniDumpWithDataSegs, &ExpParam, NULL, NULL);

	return EXCEPTION_EXECUTE_HANDLER;
}
#endif

namespace P3D
{
	static wchar szExePath[MAX_PATH];

	void Fn3()
	{
		//PROF(Fn3);
	}

	void Fn2()
	{
		//PROF(Fn2);
		Fn3();
	}

	void Fn1()
	{
		//PROF(Fn1);
		Fn2();
		Fn3();
		Fn2();
	}

	void ODS(const wchar* msg)
	{
		OutputDebugString(msg);
	}

	/*SVar sv_test("testInt", "test", 12);
	SVar sv_test2("testIntStr", "test", 12, "", SV_STRING);
	SVar sv_test3("testFloat", "test", 12.12f);
	SVar sv_test4("testStr", "test", "Test string");
	SVar sv_test5("testPtr", "test", (void*)123, "", SV_POINTER); */

	void del(void* data) { delete[] (BYTE*)data; }; // for sqlite test

	//my thread tests
	class MyRunnable : public CRunnable
	{
	public:
		void Run()
		{
			char str[256];
			//for(UINT i=0; i<1000; i++) printf("Hello #%d\n", i);
			for(UINT i=0; i<1000; i++) 
			{
				sprintf(str, "Hello #%d\n", i);
				OutputDebugString(_A2W(str));
			}
		}
	};
	CThread *thread1;

	int RunMain( void* hInst, void* hi, wchar* lpCmdLine, int iMain)
	{
		// memory leaks detection
#if defined(_DEBUG) && defined(_MSC_VER) && _MSC_VER >= 800
		_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
		//_CrtSetBreakAlloc(76);
#endif

		// PROFILER TEST
		Fn1();
		//const CProfilerNode* nod = s_prof.GetRootNode();

		// --------------- ARRAYS AND STRING TESTING -----------------------
		/*tArray<int> arr(5);
		arr.Add(1);
		arr.Add(23);
		arr.Add(43);
		arr.Add(62);
		arr.Add(36);
		//arr.Add(245); // array range exceeded
		arr.AddEx(245); // array capacity is automatically expanded ;)

		Str ts(128);

		ts.Set("Pokusny string s pevnou delkou");
		 MessageBox(0, ts, "String test", 0);
		ts+=", ktery umi taky spojovani retezcu";
		 MessageBox(0, ts, "String test", 0);
		ts<<" a taky proudy. Cislo: " << 1345 << ", desetinne: " << 35.3098f;
		 MessageBox(0, ts, "String test", 0);
		ts.Format("A formatovany vstup %s... Napr. cislo: %d, desetinne cislo: %f", "je taky mozny", 12345, 3465.342f);
		 MessageBox(0, ts, "String test", 0);
		ts = "Taky jde zkratit buffer na presny pocet znaku ktere v nem jsou."; ts.ClearUnusedBuffer();
		 MessageBox(0, ts, "String test", 0);

	    // try some expandable string functions
		Str tse; // allocate empty string
		tse.SetEx("Expanded buffer");
		 MessageBox(0, tse, "String test", 0);
		tse.SetEx("Longer expanded buffer");
		 MessageBox(0, tse, "String test", 0);

		// get hash
	    tse.FormatEx("Hash256 of '%s' is %d", ts, ts.Hash256());
		 MessageBox(0, tse, "String test", 0);

	    int breakk = 0;*/

		/*Str sarr;
		tArray<int> arr;
		srand(23624);
		while(arr.Size()!=2048)
		{
			int r = rand() % 2048;
			if (arr.FindIndex(r)<0) arr.AddEx(r);
		}

		sarr.SetEx("wchar g_randHash2048[2048] = {\r\n");
		int i=0;
		for (tArray<int>::Iterator it=arr.Begin();it!=arr.End();it++) 
		{
			if (i>29)
			{
				i = 0;
				sarr.AppendEx("\r\n");
			}
			Str tmp(10);
			tmp.Format("%d, ", *it);
			sarr.AppendEx(tmp);
			i++;
		}
		sarr.AppendEx("};");*/

		// ------------------------------------------------------------------------------------------------

		// LOAD DLL MODULES FROM EXE PATH
#if defined(WIN32)
		GetModuleFileName((HMODULE)hInst, szExePath, MAX_PATH);
		for(int i=(int)wstrlen(szExePath); i > 0; i--)
		{
			if (szExePath[i]=='/' || szExePath[i]=='\\')
			{
				szExePath[i]=0;
				break;
			}
		}
#elif defined(LINUX)
		// we have szExePath already defined in main
#endif

		// ---------------------------------
		I_Initialize(szExePath); // initialize dll module system
		// ---------------------------------


		/** TEST: sqlite3 */
		#define DBCALL(db, cmd) if (##cmd##!=SQLITE_OK) OutputDebugString( _A2W(sqlite3_errmsg(db)) ); /*CON(MSG_ERR, "DB Error: %s", _A2W(sqlite3_errmsg(db)))*/
		// TODO: sqlite3_open uses UTF-8 filename encoding. Better use our wide path and than convert it to utf-8 for sqlite
		/*sqlite3 *db=NULL; char szPhysPath[MAX_PATH];
		strcpy( szPhysPath, _W2A(I_GetBinaryPath()) );
		strcat( szPhysPath, "../data/main.rdb" );
		sqlite3_open( szPhysPath, &db );
		
		sqlite3_stmt* stmt=NULL;*/
		DBCALL( db, sqlite3_prepare_v2(db, "CREATE TABLE test (id INT PRIMARY KEY, str TEXT, bin BLOB NULL);", -1, &stmt, NULL) );
		//DBCALL( db, sqlite3_prepare_v2(db, "INSERT INTO test (str) VALUES ('first');", -1, &stmt, NULL) );
		/*DBCALL( db, sqlite3_prepare_v2(db, "INSERT INTO test (str, bin) VALUES ('binary', ?);", -1, &stmt, NULL) );
		BYTE* xdata; DWORD xlen;
		FILE* xfp = fopen("d:/3 Doors Down - Here Without You.mp4", "rb");
		fseek(xfp, 0, SEEK_END);
		xlen = ftell(xfp);
		fseek(xfp, 0, SEEK_SET);
		xdata = new BYTE[xlen];
		fread(xdata, 1, xlen, xfp);
		fclose(xfp);
		DBCALL( db, sqlite3_bind_blob(stmt, 1index of question mark starting from 1, xdata, xlen, &del) );*/
		/*DBCALL( db, sqlite3_prepare_v2(db, "SELECT id, str FROM test;", -1, &stmt, NULL) );
		const char* tmp;
		while (sqlite3_step(stmt) == SQLITE_ROW)
			OutputDebugString( _A2W( (! (tmp=(const char*)sqlite3_column_text(stmt, 1)) ) ? "(null)" : tmp  ) );
		DBCALL( db, sqlite3_finalize(stmt) );

		sqlite3_close(db);
		int dummy;*/
		/** /TEST: sqlite3 */


		/** TEST: ICU */
		  /*UChar source[] = { 0x041C, 0x043E, 0x0441, 0x043A, 0x0432,
                     0x0430, 0x0021, 0x0000 };*/
		  wchar source[] = _W("\x041C\x043E\x0441\x043A\x0432\x0430\x0021");  // "Moskva!" v azbuce
		  //wchar source[] = _W("Kiminari Yamamoto");

		  //char target[100];
		  //UErrorCode status = U_ZERO_ERROR;
		  //UConverter *conv;
		  //int32_t     len;

		  //Transliterator *t = 0;
		  //t = Transliterator::createInstance("Any-Latin", UTRANS_FORWARD, status); // "Any-Latin"
		  //UnicodeString ustr;
		  //ustr = source;
		  //t->transliterate( ustr );

		  //// set up the converter
		  //conv = ucnv_open("utf-8", &status);
		  //assert(U_SUCCESS(status)!=0);

		  //// convert 
		  ///*len = ucnv_fromUChars(conv, target, 100, ustr, -1, &status);
		  //assert(U_SUCCESS(status)!=0);*/
		  //len = ustr.extract(target, 100, conv, status);

		  //FILE* fp = fopen("D:\\testKatakana.txt", "wb");
		  //fwrite(target, 1, len, fp);
		  //fclose(fp);

		  //// close the converter
		  //ucnv_close(conv);
		/** /TEST: ICU */


		// FILESYSTEM TEST
		/*
		FSFILE* fp = Filesystem.Open("test.txt", "rb");
		if (fp)
		{
			// here you can manipulate with this file using Filesystem.Write, Filesystem.Read...
			Filesystem.Close(fp); // I will close it to demonstrate loading of all content from file

			BYTE* pData; unsigned long size=0;
			FSFILE* fpp = Filesystem.Load("test.txt", pData, size, true);
			CON(MSG_INFO, "Content of Test.txt: %s", pData);
			Filesystem.UnLoad(fpp, pData);
		}
		else
		{
			CON(MSG_ERR, "Can't load test.txt using filesystem. Try to create data/test.txt ;)");
		}
		*/

		// ------------------------------------------------------------------------------------------------

		// --------------- INITIALIZE ENGINE -----------------------

		IEngine *pEngine = (IEngine*)I_GetModule(_W("engine"), NULL);
		assertd(pEngine!=0, "Engine could not be loaded! Missing Panther3D.dll?");
		pEngine->Initialize();


		//pEngine->iConsole()->Msg(MSG_BOX, "Test");
		/*IAppWindow *pWindow = pEngine->iAppWindow();*/

		// --------------- INITIALIZE RENDERER -----------------------

		IRenderer* g_pRenderer = pEngine->mRenderer();
		ISoundEngine* g_pSound = pEngine->mSound();




		IGraphicsManager* g_pGraphicsManager = g_pRenderer->iGraphicsManager();
		IGraphicsDevice* g_pGraphicsDevice = g_pRenderer->iGraphicsDevice();
		// nebo primo




		//assert(newObj!=0); // Another example of asser, we assert, that newObj!=0.
		// If it will be false, assertion failed in debug config. will be shown.
		IPrimitiveRenderer* g_pPrimitiveRenderer = g_pRenderer->iPrimitiveRenderer();
		ITextRenderer* g_pTextRenderer = g_pRenderer->iTextRenderer();
		IGraphicsRenderer *g_pGraphicsRenderer = g_pRenderer->iGraphicsRenderer();

		//ISpriteRenderer * g_pSpriteRenderer = g_pRenderer->iSpriteRenderer();



		//ITexture* testtex = g_pRenderer->iGraphicsManager()->LoadTexture("aaa.jpg");

// 		MyRunnable *runnable1 = new MyRunnable;
// 		sThreadDesc thread1desc;
// 		thread1desc.priority = P3D_THREAD_PRIORITY_NORMAL;
// 		thread1 = new CThread(runnable1, thread1desc, true);
// 		thread1->Launch();

// 		sRasterizerDesc rd;
// 		rd.Wireframe = false;
// 		IRasterizerState *rs = g_pGraphicsDevice->CreateRasterizerState(&rd);

		/*g_pSound->TestSoundModule();

		g_pSound->OpenStream("test.ogg");

		g_pSound->Playback();*/

		/*g_pSound->GetManager()->PrecacheSound("music3");
		ISoundSource *ssource = g_pSound->GetManager()->CreateSource("music2", Vec3(), false);
		SoundReverbProperties props = REVERB_PRESET_ICEPALACE_HALL;
		ssource->SetReverb(props);
		ssource->SetSourceRelativity(false);
		ssource->SetVelocity(0,2,0);
		ssource->SetPosition(0,0,10);
		ssource->Play();*/

		ISceneManager* g_pSceneManager = 0;
		g_pSceneManager = pEngine->iSceneManager();

		ICamera *camera = g_pSceneManager->CreateCamera(Vec3(0.0f, 23.0f, 10.0f), Vec3(0.0f, 0.0f, 0.0f), true);


		g_pSceneManager->SetActiveCamera(camera);
		/*IActor * actor =  g_pSceneManager->CreateActor( _W("dwarf1"), Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 3.14f, 0.0f), Vec3(1.0f, 1.0f, 1.0f));
		//!!vypnul sem pro jistotu culling
		ITexture *atex = g_pGraphicsManager->LoadTexture( _W("axe") );
		ITexture *atex2 = g_pGraphicsManager->LoadTexture( _W("dwarf") );
		if (actor)
		{
		actor->SetTexture(0, atex);
		actor->SetTexture(1, atex2);
		}

		IActor * cubes[4];
		for(unsigned int i=0; i<4; i++) cubes[i] = g_pSceneManager->CreateActor( _W("cube"), Vec3(-10.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 0.0f), Vec3(0.02f, 0.02f, 0.02f));

		IActor * sphere =  g_pSceneManager->CreateActor( _W("sphere"), Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 0.0f), Vec3(1.0f, 1.0f, 1.0f));

		IActor *map = g_pSceneManager->CreateActor( _W("map"), Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 0.0f), Vec3(0.05f, 0.05f, 0.05f));
		
		map->SetTexture(0, g_pGraphicsManager->LoadTexture( _W("maptex") ));
*/
		IPhysEngine *g_pPhysEngine = pEngine->mPhys();
		IPhysManager *g_pPhysManager = g_pPhysEngine->iPhysManager();
/*		IPhysBody *body = g_pPhysManager->CreateBody(actor->GetAABB(), Vec3(0.0f, 20.0f, 0.0f), true);
		AABB floorsize(Vec3(-20.0f, -10.0f, -20.0f), Vec3(20.0f, 0.0f, 20.0f));
		//IPhysBody *floor = g_pPhysManager->CreateBody(floorsize, Vec3(0.0f, 0.0f, 0.0f), false);
		sPhysControllerDesc cdesc;
//		IPhysCharacter *character = g_pPhysManager->CreateCharacter(cdesc, Vec3(-10.0f, 20.0f, 0.0f));
		//body->SetKinematic(true);

		IPhysBody *cubebodies[4];
		cubebodies[0] = g_pPhysManager->CreateBody(cubes[0]->GetAABB(), Vec3(-3.0f, 40.0f, -3.0f), true);
		cubebodies[1] = g_pPhysManager->CreateBody(cubes[0]->GetAABB(), Vec3(-3.0f, 40.0f, 3.0f), true);
		cubebodies[2] = g_pPhysManager->CreateBody(cubes[0]->GetAABB(), Vec3(3.0f, 40.0f, -3.0f), true);
		cubebodies[3] = g_pPhysManager->CreateBody(cubes[0]->GetAABB(), Vec3(3.0f, 40.0f, 3.0f), true);

		IPhysBody *spherebody = g_pPhysManager->CreateBody(sphere->GetAABB(), Vec3(-1.0f, 10.0f, 0.0f), true);
		sSphereShapeDesc spheredesc;
		spheredesc.radius = sphere->GetAABB().GetExtent(0);
		spherebody->SetShape(&spheredesc);

		IPhysTriangleMesh * church = g_pPhysManager->CreateTriangleMesh( _W("map") , Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 0.0f), Vec3(0.05f, 0.05f, 0.05f));
*/
		sTextureDesc desc;
		desc.ArraySize = 1;
		desc.AutoGenerateMips = false;
		desc.BindFlag = P3D_TEXBINDFLAG_RENDER_TARGET;
		desc.CPUAccess = P3D_CPUACCESS_NONE;
		desc.Format = P3D_FORMAT_X8R8G8B8_UNORM;
		desc.MipLevels = 1;
		desc.ResourceUsage = P3D_RESOURCEUSAGE_DEFAULT;
		desc.Width = 1024;
		desc.Height = 576;
		ITexture *rt = g_pGraphicsManager->CreateTexture(desc, 0, 0);

		sTextureDesc depthdesc;
		depthdesc.ArraySize = 1;
		depthdesc.AutoGenerateMips = false;
		depthdesc.BindFlag = P3D_TEXBINDFLAG_DEPTH_STENCIL;
		depthdesc.CPUAccess = P3D_CPUACCESS_NONE;
		depthdesc.Format = P3D_FORMAT_D24S8;
		depthdesc.MipLevels = 1;
		depthdesc.ResourceUsage = P3D_RESOURCEUSAGE_DEFAULT;
		depthdesc.Width = 1024;
		depthdesc.Height = 576;
		ITexture *depthbuffer = g_pGraphicsManager->CreateTexture(depthdesc, 0, 0);

		sRenderCanvasDesc rwd;
		rwd.size = Vec2i(1024, 576);
		rwd.windowed = true;

		IRenderCanvas *rw = g_pGraphicsDevice->CreateRenderWindow(rwd, GetActiveWindow());

		//my thread test
// 		thread1->Wait();
// 		delete thread1;
		
		//ISoundSource *music = 0;
		bool bRunning=true;
		do
		{
			if ( pEngine->iInputManager()->IsKeyDownFirstTime(KEY_ESCAPE) )
			{
				break;
			}

			/*if ( pEngine->iInputManager()->IsKeyDownFirstTime(KEY_M) )
			{
				if(music)
				{
					music->Stop();
					pEngine->mSound()->GetManager()->DestroySource(music);
				}

				music = pEngine->mSound()->GetManager()->CreateSource("music3",Vec3(rand()%50,rand()%50,rand()%50), false);

				if(music)
					music->Play();
			}*/

			g_pPhysEngine->SimulationBegin(pEngine->GetDeltaTime());
			//#/*if(body->Changed())*/ actor->SetPosRotMatrix(*body->GetMatrix());
			//#for(unsigned int i=0; i<4; i++) cubes[i]->SetPosRotMatrix(*cubebodies[i]->GetMatrix());
			//#sphere->SetPosRotMatrix(*spherebody->GetMatrix());
			g_pPhysEngine->SimulationEnd();			
			
			

			//bRunning = pEngine->BeginScene();
			//g_pGraphicsDevice->BeginScene(rw, ColorRGB(0, 0.5f, 0.5f));
			//g_pGraphicsRenderer->RenderScene();
			bRunning = pEngine->NextFrame();


		} while (bRunning);


		//rs->Release();

		//g_pSound->ReleaseStream();

		

		// ---------------------------------
		I_Shutdown();
		// ---------------------------------

		return 0;
	}

};


#if defined(WIN32)

#ifdef _UNICODE
int __stdcall wWinMain(HINSTANCE hInst, HINSTANCE hi, wchar* lpCmdLine, int iMain)
#else
int __stdcall WinMain( HINSTANCE hInst, HINSTANCE hi, LPSTR* lpCmdLine, int iMain)
#endif
{
#if !defined(_DEBUG)
	__try
	{
#endif

	P3D::RunMain(hInst, hi, lpCmdLine, iMain);

#if !defined(_DEBUG)
	}
	__except(GenerateDump(GetExceptionInformation())){}
#endif
}
#elif defined(LINUX)
int main(int argc, wchar **argv)
{
	// get executable path
	wstrcpy(P3D::szExePath, argv[0]);
	for (int i=wstrlen(P3D::szExePath); i>0; i--)
	{
		if (P3D::szExePath[i]=='/')
		{
			P3D::szExePath[i+1]=0;
			break;
		}
	}

	// get command line
	wchar cmdLine[512]=""; int currLen=0;
	for (int i=1; i<argc; i++)
	{
		if (wstrlen(argv[i])+1>512)
		{
			cmdLine[currLen]=0;
			break; // too long
		}

		wstrcpy(&cmdLine[currLen], argv[i]);
		currLen += wstrlen(argv[i]);

		if (i==argc-1)
			cmdLine[currLen]=0;
		else
			cmdLine[currLen]=' ';

		currLen++;
	}

	return P3D::RunMain(0, 0, cmdLine, 1);
}
#endif
