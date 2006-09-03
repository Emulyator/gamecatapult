# Microsoft Developer Studio Project File - Name="ExportX" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** 編集しないでください **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=ExportX - Win32 Debug
!MESSAGE これは有効なﾒｲｸﾌｧｲﾙではありません。 このﾌﾟﾛｼﾞｪｸﾄをﾋﾞﾙﾄﾞするためには NMAKE を使用してください。
!MESSAGE [ﾒｲｸﾌｧｲﾙのｴｸｽﾎﾟｰﾄ] ｺﾏﾝﾄﾞを使用して実行してください
!MESSAGE 
!MESSAGE NMAKE /f "ExportX.mak".
!MESSAGE 
!MESSAGE NMAKE の実行時に構成を指定できます
!MESSAGE ｺﾏﾝﾄﾞ ﾗｲﾝ上でﾏｸﾛの設定を定義します。例:
!MESSAGE 
!MESSAGE NMAKE /f "ExportX.mak" CFG="ExportX - Win32 Debug"
!MESSAGE 
!MESSAGE 選択可能なﾋﾞﾙﾄﾞ ﾓｰﾄﾞ:
!MESSAGE 
!MESSAGE "ExportX - Win32 Release" ("Win32 (x86) Application" 用)
!MESSAGE "ExportX - Win32 Debug" ("Win32 (x86) Application" 用)
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ExportX - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /I "C:\LightWave\lwsdk75\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "C:\LightWave\lwsdk75\include" /I "../../Scn3" /I "../../Obj2" /I "../meshio" /I "../../../include" /D "NDEBUG" /D "_MT" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_LITTLE_ENDIAN" /D "GCTP_USE_XFILEAPI" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"LIBCD" /out:"Release/ExportX.p"
# ADD LINK32 user32.lib /nologo /dll /machine:I386 /out:"C:\LightWave_8\Plugins\Input-Output\ExportX.p"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "ExportX - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /I "C:\LightWave\lwsdk75\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "C:\LightWave\lwsdk75\include" /I "../../Scn3" /I "../../Obj2" /I "../meshio" /I "../../../include" /D "_DEBUG" /D "_MT" /D "_STLP_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_LITTLE_ENDIAN" /D "GCTP_USE_XFILEAPI" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"LIBC" /out:"Debug/ExportX.p" /pdbtype:sept
# ADD LINK32 user32.lib /nologo /dll /debug /machine:I386 /out:"C:\LightWave_8\Plugins\Input-Output\ExportX.p" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "ExportX - Win32 Release"
# Name "ExportX - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\activate.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Scn3\bone.c
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\channel.c
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\chunk.c
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\clip.c
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\color.c
# End Source File
# Begin Source File

SOURCE=..\..\Scn3\curve.c
# End Source File
# Begin Source File

SOURCE=.\dxani.cpp
# End Source File
# Begin Source File

SOURCE=.\dxcfg.cpp
# End Source File
# Begin Source File

SOURCE=.\dxexp.cpp
# End Source File
# Begin Source File

SOURCE=.\dxffp.cpp
# End Source File
# Begin Source File

SOURCE=.\dxfrm.cpp
# End Source File
# Begin Source File

SOURCE=.\dxmat.cpp
# End Source File
# Begin Source File

SOURCE=.\dxmsh.cpp
# End Source File
# Begin Source File

SOURCE=.\dxopt.cpp
# End Source File
# Begin Source File

SOURCE=.\dxskm.cpp
# End Source File
# Begin Source File

SOURCE=.\dxutl.cpp
# End Source File
# Begin Source File

SOURCE=.\dxval.cpp
# End Source File
# Begin Source File

SOURCE=.\dxvec.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\edge.c
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\envl.c
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\fileio.c
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\global.c
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\layer.c
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\lwo2_c.c
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\lwo2_e.c
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\lwo2_l.c
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\lwo2_m.c
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\lwo2_s.c
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\lwo2_x.c
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\lwob_l.c
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\lwob_m.c
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\lwob_s.c
# End Source File
# Begin Source File

SOURCE=..\..\Scn3\lws1.c
# End Source File
# Begin Source File

SOURCE=..\..\Scn3\lws3.c
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\material.c
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\mathlib.c
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\memcon.c
# End Source File
# Begin Source File

SOURCE=..\Meshio\memio.c
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\mesh.c
# End Source File
# Begin Source File

SOURCE=..\Meshio\meshio.c
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\misc.c
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\normal.c
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\obj2.c
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\objdump.c
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\point.c
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\polygon.c
# End Source File
# Begin Source File

SOURCE=..\..\Scn3\scn3.c
# End Source File
# Begin Source File

SOURCE=..\..\Scn3\scndump.c
# End Source File
# Begin Source File

SOURCE=.\serv.def
# End Source File
# Begin Source File

SOURCE=.\servmain.c
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\share.c
# End Source File
# Begin Source File

SOURCE=.\shutdown.c
# End Source File
# Begin Source File

SOURCE=.\startup.c
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\surface.c
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\tags.c
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\triple.c
# End Source File
# Begin Source File

SOURCE=.\ui_expo.cpp
# End Source File
# Begin Source File

SOURCE=.\ui_img.cpp
# End Source File
# Begin Source File

SOURCE=.\ui_key.cpp
# End Source File
# Begin Source File

SOURCE=.\ui_main.cpp
# End Source File
# Begin Source File

SOURCE=.\ui_util.cpp
# End Source File
# Begin Source File

SOURCE=.\username.c
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\uview.c
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\uvmap.c
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\vmad.c
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\vmap.c
# End Source File
# Begin Source File

SOURCE=.\wrani.cpp
# End Source File
# Begin Source File

SOURCE=.\wrfrm.cpp
# End Source File
# Begin Source File

SOURCE=.\wrmat.cpp
# End Source File
# Begin Source File

SOURCE=.\wrmsh.cpp
# End Source File
# Begin Source File

SOURCE=.\wrscn.cpp
# End Source File
# Begin Source File

SOURCE=.\wrtmp.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\Scn3\bone.h
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\channel.h
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\chunk.h
# End Source File
# Begin Source File

SOURCE=..\..\Scn3\curve.h
# End Source File
# Begin Source File

SOURCE=.\dxexp.h
# End Source File
# Begin Source File

SOURCE=.\dxopt.h
# End Source File
# Begin Source File

SOURCE=.\dxtyp.h
# End Source File
# Begin Source File

SOURCE=.\dxui.h
# End Source File
# Begin Source File

SOURCE=.\dxutl.h
# End Source File
# Begin Source File

SOURCE=.\dxvec.h
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\lwo2.h
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\lwob.h
# End Source File
# Begin Source File

SOURCE=..\..\Scn3\lws1.h
# End Source File
# Begin Source File

SOURCE=..\..\Scn3\lws3.h
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\matdef.h
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\mathlib.h
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\memcon.h
# End Source File
# Begin Source File

SOURCE=..\Meshio\memio.h
# End Source File
# Begin Source File

SOURCE=..\Meshio\meshio.h
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\misc.h
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\obj2.h
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\objdef.h
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\prot_chn.h
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\prot_clp.h
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\prot_col.h
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\prot_dmp.h
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\prot_edg.h
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\prot_env.h
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\prot_fio.h
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\prot_glo.h
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\prot_lay.h
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\prot_mat.h
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\prot_msh.h
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\prot_nml.h
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\prot_pnt.h
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\prot_pol.h
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\prot_shr.h
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\prot_srf.h
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\prot_tag.h
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\prot_tri.h
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\prot_uvi.h
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\prot_uvm.h
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\prot_vmd.h
# End Source File
# Begin Source File

SOURCE=..\..\Obj2\prot_vmp.h
# End Source File
# Begin Source File

SOURCE=..\..\Scn3\scn3.h
# End Source File
# Begin Source File

SOURCE=..\..\Scn3\scndump.h
# End Source File
# Begin Source File

SOURCE=.\ui_util.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
