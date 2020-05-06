var objGlobal = new Object;
objGlobal.initialized = false;

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
    debugger
    var argsArray = this.loadTool.arguments;
};
