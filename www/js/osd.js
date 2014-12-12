/////////////////////////////////////////////////////////////////////
//
// osd.js
//
/////////History:
//	2014/11/26 - [Sean Hou] Created this file
//
// Copyright (C) 2012 - 2030, GOKE, Inc.
//
// All rights reserved. No Part of this file may be reproduced, stored
// in a retrieval system, or transmitted, in any form, or by any means,
// electronic, mechanical, photocopying, recording, or otherwise,
// without the prior consent of GOKE, Inc.
//
/////////////////////////////////////////////////////////////////////
var paint = false;
var context;
var colorBrown = "#986928";

var canvasWidth = 800;
var canvasHeight = 600;
var MAX_RECT = 4;

function point()//class
{
    this.x=[];
    this.y=[];
    this.w=[];
    this.h=[];
    this.count=0;
    this.tmpx;
    this.tmpy;
    this.tmpw;
    this.tmph;
    this.addtmpwh = function(tw, th)//一定要这样么
    {
        this.tmpw = tw;
        this.tmph = th;
    }
    this.addtmpxy = function(tx, ty)
    {
        this.tmpx = tx;
        this.tmpy = ty;
    }
    this.addtmp = function()
    {
        //cover int to unsigned int
        if (this.tmpw < 0)
        {
            this.tmpx += this.tmpw;
            this.tmpw = 0 - this.tmpw;
        }
        if (this.tmph < 0)
        {
            this.tmpy += this.tmph;
            this.tmph = 0 - this.tmph;
        }

        this.x.push(this.tmpx);
        this.y.push(this.tmpy);
        this.w.push(this.tmpw);
        this.h.push(this.tmph);
        this.count++;
    }
    this.clear = function()
    {
        this.x.splice(0, this.x.length);
        this.y.splice(0, this.y.length);
        this.w.splice(0, this.w.length);
        this.h.splice(0, this.h.length);
        this.count = 0;
    }
}

var drawp = new point();
/**
* Clears the canvas.
*/
function clearCanvas()
{
	context.clearRect(0, 0, canvasWidth, canvasHeight);
}
/**
* interface to redraw
*/
function outside_redraw()
{
//    context.save();
    context.fillStyle = colorBrown;
    if (drawp.count > MAX_RECT)
    {
        drawp.clear();
    }
    for (var i=0; i<drawp.count; i++)
    {
        context.fillRect(drawp.x[i], drawp.y[i], drawp.w[i], drawp.h[i]);	
    }
 
    drawstroke();
}

function redraw()
{
    clearCanvas();
    context.fillStyle = colorBrown;
    if (drawp.count > MAX_RECT)
    {
        drawp.clear();
    }
    for (var i=0; i<drawp.count; i++)
    {
        context.fillRect(drawp.x[i], drawp.y[i], drawp.w[i], drawp.h[i]);	
    }
 
    context.fillRect(drawp.tmpx, drawp.tmpy, drawp.tmpw, drawp.tmph);	
    drawstroke();
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
	canvas.setAttribute('id', 'canvas_test');
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
	$('#canvas_test').mousedown(function(e)
	{
		// Mouse down location
		var mouseX = e.pageX - this.offsetLeft;
		var mouseY = e.pageY - this.offsetTop;

        drawp.addtmpxy(mouseX, mouseY);
        paint = true;

    });
	$('#canvas_test').mousemove(function(e){
		if(paint==true){
            var mouseX = e.pageX - this.offsetLeft;
            var mouseY = e.pageY - this.offsetTop;

            drawp.addtmpwh(mouseX - drawp.tmpx, mouseY - drawp.tmpy);
			redraw();//画前面几个和当前的这个
		}
	});
	
	$('#canvas_test').mouseup(function(e){
        var mouseX = e.pageX - this.offsetLeft;
        var mouseY = e.pageY - this.offsetTop;

        drawp.addtmpwh(mouseX - drawp.tmpx, mouseY - drawp.tmpy);
	  	redraw();
        drawp.addtmp();
        paint = false;
	});
    
}
