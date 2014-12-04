var paint = false;
var context;
var colorBrown = "#986928";

var tmpX ;//= new Array();
var tmpY ;//= new Array();
var tmpW ;//= new Array();
var tmpH ;//= new Array();
var	clickX = new Array();
var	clickY = new Array();
var	widthX = new Array();
var	heightY = new Array();

var canvasWidth = 800;
var canvasHeight = 600;

function point()
{
    var x=[];
    var y=[];
    var w=[];
    var h=[];
    var count=0;
    var tmpx;
    var tmpy;
    var tmpw;
    var tmph;
    this.addtmpwh = function(tw, th)//一定要这样么
    {
        tmpw = tw;
        tmph = th;
    }
    function addtmpxy(tx, ty)
    {
        tmpx = tx;
        tmpy = ty;
    }
    function addtmp()
    {
        x.push(tmpx);
        y.push(tmpy);
        w.push(tmpw);
        h.push(tmph);
        count++;
    }
    function clear()
    {
        x.splice(0, x.length);
        y.splice(0, y.length);
        w.splice(0, w.length);
        h.splice(0, h.length);
    }
}

function addXY(x, y)
{
    tmpX = x;
    tmpY = y;
}

function addWH(w, h)
{
    tmpW = w;
    tmpH = h;
}

/**
* Clears the canvas.
*/
function clearCanvas()
{
	context.clearRect(0, 0, canvasWidth, canvasHeight);
}

function redraw()
{
    clearCanvas();
    var i;
    for (i=0; i<clickX.length; i++)
    {
        context.fillStyle = colorBrown;
        context.fillRect(clickX[i], clickY[i], widthX[i], heightY[i]);	
//        context.beginPath();
//        context.moveTo(clickX[i], clickY[i]);
//        context.lineTo(clickX[i] + widthX[i], clickY[i]);
//        context.lineTo(clickX[i] + widthX[i], clickY[i]+heightY[i]);
//        context.closePath();
//        context.strokeStyle="green";
//        context.stroke();       

    }

    context.fillStyle = colorBrown;
    context.fillRect(tmpX, tmpY, tmpW, tmpH);	
    drawstroke();
//    context.beginPath();
//    context.moveTo(clickX[i], clickY[i]);
//    context.lineTo(clickX[i] + widthX[i], clickY[i]);
//    context.lineTo(clickX[i] + widthX[i], clickY[i]+heightY[i]);
//    context.closePath();
//    context.strokeStyle="green";
//    context.stroke();       
}
/**
* Adds a point to the drawing array.
* @param x
* @param y
* @param dragging
*/
function addClick(x, y, dragging)
{
	clickX.push(x);
	clickY.push(y);
}

function addClickwh(x, y, dragging)
{
	widthX.push(x);
	heightY.push(y);

//	clickTool.push(curTool);
//	clickColor.push(curColor);
//	clickSize.push(curSize);
//	clickDrag.push(dragging);
}
function drawstroke()
{
    context.rect(0, 0, canvasWidth, canvasHeight);
    context.strokeStyle="green";
    context.stroke();       
}
function prepareCanvas()
{
	// Create the canvas (Neccessary for IE because it doesn't know what a canvas element is)
	var canvasDiv = document.getElementById('canvasDiv');
	canvas = document.createElement('canvas');
	canvas.setAttribute('width', canvasWidth);
	canvas.setAttribute('height', canvasHeight);
	canvas.setAttribute('id', 'canvas');
	canvasDiv.appendChild(canvas);
	if(typeof G_vmlCanvasManager != 'undefined') {
		canvas = G_vmlCanvasManager.initElement(canvas);
	}
	context = canvas.getContext("2d"); // Grab the 2d canvas context
    drawstroke();
	// Note: The above code is a workaround for IE 8 and lower. Otherwise we could have used:
	//     context = document.getElementById('canvas').getContext("2d");
	
	// Add mouse events
	// ----------------
	$('#canvas').mousedown(function(e)
	{
		// Mouse down location
		var mouseX = e.pageX - this.offsetLeft;
		var mouseY = e.pageY - this.offsetTop;
        addXY(mouseX, mouseY);//tmp
        paint = true;

    });
	$('#canvas').mousemove(function(e){
		if(paint==true){
			addWH(e.pageX - this.offsetLeft - tmpX, e.pageY - this.offsetTop - tmpY, true);//tmp
			redraw();//画前面几个和当前的这个
		}
	});
	
	$('#canvas').mouseup(function(e){
        addClickwh(e.pageX - this.offsetLeft - tmpX, e.pageY - this.offsetTop - tmpY, true);

        addClick(tmpX, tmpY);
	  	redraw();
        paint = false;
	});
    
}
