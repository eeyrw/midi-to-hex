option title, "Score List (*.raw) structure"
=
=
=	Tiny Hexer script for .raw structure view in
=	mirkes.de's tiny hex editor
=
=


option GlobalVars, 1
option ReadTags, 1
option target, structureviewer

= open active editor and goto current position/0
var editor file
editor = fileopen('::current')
if ((param_count > 0) and (dword(params(0))==1))
  fileseek editor, 0
else
  fileseek editor, filegetprop(editor, 'selstart')
endif
var start dword
start = filegetprop(editor, 'position')

= open browser window
var browser file
browser = fileopen('::browser', 'c')

filesetprop browser, 'accepttags', 1
filewrite browser "<font color=",'"',"blue",'"',"><b><u>Score Data List structure:</u></b></font>\n\n"
filesetprop browser, 'accepttags', 0

filewrite browser "File: '",filegetprop(editor, 'filename'),"'\nPosition: ",start,"\n\n"


= read the Score List header
var _identifier dword _scorecount dword
fileread editor _identifier

if  _identifier != 0x45524353
  error 'Unknown file format'
endif


= print file header
var _text text
filewrite browser "struct ScoreDataList {\n"
_text = data2text(_identifier)
copytags _text _identifier
filewrite browser "  DWORD   identifer\t'",_text,"'\t\t(",_identifier,");\n"
fileread editor _scorecount
filewrite browser "  DWORD  scoreCount\t",DEC(_scorecount),";\n"
LOOP showScoreAddrList,_scorecount

LOCAL  showScoreAddrList
var _addr DWORD
fileread editor _addr
filewrite browser "  DWORD  score",DEC(loop),"addr\t",_addr,";\n"
TAGVAR _addr,_addr,1
filewrite browser "\t\t\t[",_addr,"]\n"
ENDLOCAL



