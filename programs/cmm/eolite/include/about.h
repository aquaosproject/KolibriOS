//Leency 2008-2016

#define BROWSER_PATH    "/sys/network/webview"
#define BROWSER_LINK    "http://kolibri-n.org/inf/eolite/eolite_p1"


void about_dialog()
{   
	byte id;
	proc_info about_form;
	int about_x;

	if (active_about) {cmd_free=2;ExitProcess();} else active_about=1;
	loop() switch(WaitEvent())
	{
		case evButton: 
				id=GetButtonID();
				IF (id==1)
				{
					active_about=0;
					cmd_free = 2;
					ExitProcess();
				}
				IF (id==11) RunProgram(BROWSER_PATH, BROWSER_LINK);
				break;
				
		case evKey:
				IF (GetKey()==27)
				{
					active_about=0;
					cmd_free = 2;
					ExitProcess();
				}
				break;
				
		case evReDraw:
				DefineAndDrawWindow(Form.left+Form.width/2,Form.top+Form.height/2-114,310,300+GetSkinHeight(),0x34,system.color.work,NULL);
				GetProcessInfo(#about_form, SelfInfo);
				if (about_form.status_window>2) break;
				logo_pal[0] = system.color.work;
				PutPaletteImage(#logo,86,86,about_form.cwidth-86/2,10,8,#logo_pal);
				about_x = -strlen(ABOUT_TITLE)*18+about_form.cwidth/2;
				WriteTextB(about_x+2,107,0x82,0xD49CD2,ABOUT_TITLE);
				WriteTextB(about_x,105,0x82,0x9D129D,ABOUT_TITLE);
				DrawRectangle3D(0,154,about_form.cwidth,1,system.color.work_dark,system.color.work_light);
				WriteTextLines(7,163,0x90,system.color.work_text,"KolibriOS File Manager\nAuthors: Leency, Veliant\nPunk_Joker, Pavelyakov\n(c) 2008 - 2016",20);
				#ifdef LANG_RUS
				DrawFlatButton(about_form.cwidth-180/2,about_form.cheight-38,180,26,11,"����� ࠧࠡ�⪨");
				#endif
	}
}
