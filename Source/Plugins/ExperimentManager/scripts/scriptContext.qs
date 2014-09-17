//This works
var a = 123;
Log(a);
BrainStim.executeScriptContent("Log(a);");

////This doesn't work
//BrainStim.executeScriptContent("var b=111;\n Log(a+b);");
//BrainStim.executeScriptContent("Log(b);");//This var b is not known here...

//Fixed, this does work
BrainStim.executeScriptContent("var c=222;\n Log(a+c);\n function myAdd(nNumber)\n { return c + nNumber; } \n BrainStim.saveContextState('aa');\n BrainStim.setContextState('aa');");
BrainStim.executeScriptContent("Log(c); \n Log(myAdd(3));");

a = a + 1;
Log(a);
BrainStim.executeScriptContent("myAdd = null;");

//function myAdd(nNumber)
//{
//	return c + nNumber;
//}

