var objGlobal = new Object;
objGlobal.initialized = false;
objGlobal.controlObject = null;

/* use QJSEngine */
objGlobal.loadTools = function() {

    if (!this.initialized) {
        this.controlObject = new CenterControl();
        this.controlObject.createMVCView()

        /*$$ Tree/Table/List Widget */
        //To show createQTreeWidget
        //this.controlObject.createQTreeWidget();
        //To show createQTableWidget
        //this.controlObject.createQTableWidget();
        //To show createQTableWidget
        //this.controlObject.createQListWidget();

        /*$$ Tree/Table/List View/Model*/
//        this.controlObject.createQTreeView();
//        this.controlObject.createQTableView();
//        this.controlObject.createQListView();

        //$$To show QChar Widget
        //this.controlObject.createQcharWidget();

        //$$To show MainWindow
        //this.controlObject.testSignal.connect(testSlot);
        //this.controlObject.createMainWindow();

        this.initialized = true;
    }
};

function testSlot(){
    if(this.controlObject !== null)
        this.controlObject.printMsg("called testSlot");
}



/* use QScriptEngine*/
/*
objGlobal.loadTool = function() {
    //debugger
    var argsArray = this.loadTool.arguments;

    //connect to c++ in JS
    g_pControl.testSignal.connect(objGlobal,'qsTestSlot');

    if (!this.initialized) {
        g_pControl.createMainWindow();
        this.initialized = true;
    }

};

objGlobal.qsTestSlot = function() {
    var argsArray = this.loadTool.arguments;
};

*/


