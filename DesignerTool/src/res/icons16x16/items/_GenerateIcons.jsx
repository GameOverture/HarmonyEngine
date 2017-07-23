﻿// NAME: 
//	SaveLayers

// DESCRIPTION: 
//	Saves each layer in the active document to a PNG or JPG file named after the layer. 
//	These files will be created in the current document folder.

// REQUIRES: 
//	Adobe Photoshop CS2 or higher

// VERSIONS:
//  23 July 2017 by Jason Knobler
//		Changing functionality to combine sub layers on a single base image using my specific .psd layer names

// 	24 May 2013 by Johannes Walter  (graphicdesign.stackexchange/users/408/johannes)
// 		Nesting properly handled
// 		All layers save seperately again (no more stacking).
//		Allows selecting layer sets to export instead of all layers.

//	27 March 2013 by Robin Parmar (robin@robinparmar.com)
//		preferences stored in object
//		auto-increment file names to prevent collisions
//		properly handles layer groups
//		header added
//		code comments added
//		main() now has error catcher
//		counts number of layers
//		many little code improvements

//	26 Sept 2012 by Johannes Walter on stackexchange (graphicdesign.stackexchange/users/408/johannes)
//		Original version

// enable double-clicking from Finder/Explorer (CS2 and higher)
#target photoshop
app.bringToFront();

var baseFileName = "";

function main() {
    // two quick checks
	if(!okDocument()) {
        alert("Document must be saved and be a layered PSD.");
        return; 
    }
	//var len = activeDocument.layers.length;
    //var ok = confirm("Note: All layers will be saved in same directory as your PSD.\nThis document contains " + len + " top level layers.\nBe aware that large numbers of layers may take some time!\nContinue?");
    //if(!ok) return

	// user preferences
	prefs = new Object();
	prefs.fileType = "PNG";
	prefs.fileQuality = 12;
	prefs.filePath = app.activeDocument.path;
	prefs.count = 0;
	prefs.layersToExport = "All Layers";
    //instantiate dialogue
    //Dialog();
    hideLayers(activeDocument);
	
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// CODE CHANGE: JASON KNOBLER - Harmony Engine custom export ////////////////////////////////////////////////////////////////////////////
	var baseImageLayer = 0;
	var subIconsLayer = 0;
	var len = activeDocument.layers.length;
	for (var i = 0; i < len; i++)
	{
		if (activeDocument.layers[i].typename == 'LayerSet' && activeDocument.layers[i].name == 'BaseImage')
			baseImageLayer = activeDocument.layers[i];
		
		if (activeDocument.layers[i].typename == 'LayerSet' && activeDocument.layers[i].name == 'SubIcons')
			subIconsLayer = activeDocument.layers[i];
	}
	
	hideLayers(baseImageLayer);
	saveLayers(baseImageLayer);	// Save base images without sub-icons first
	var len = baseImageLayer.layers.length;
	for (var i = 0; i < len; i++)
	{
        var layer = baseImageLayer.layers[i];
        layer.visible = true;
		baseFileName = layer.name;
		
		saveLayers(subIconsLayer);	// Saving base image + sub-icons
		layer.visible = false;
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	
	
	//if(prefs.layersToExport == "All Layers") { saveLayers(activeDocument); }
	//else {saveFolder(activeDocument); }
	toggleVisibility(activeDocument);
    alert("Saved " + prefs.count + " files.");
}

function hideLayers(ref) {
	var len = ref.layers.length;
	for (var i = 0; i < len; i++) {
		var layer = ref.layers[i];
		if (layer.typename == 'LayerSet') hideLayers(layer);
		else layer.visible = false;
	}
}

function toggleVisibility(ref) {
	var len = ref.layers.length;
	for (var i = 0; i < len; i++) {	
		layer = ref.layers[i];
		if (layer.typename == 'LayerSet') layer.visible = true;
		else layer.visible = !layer.visible;
	}
}

function saveLayers(ref) {
	var len = ref.layers.length;
	// rename layers top to bottom
	for (var i = 0; i < len; i++) {
        var layer = ref.layers[i];
        if (layer.typename == 'LayerSet') {
            // recurse if current layer is a group
            hideLayers(layer);
			saveLayers(layer);
			
        } else {
            // otherwise make sure the layer is visible and save it
            layer.visible = true;
            saveImage(layer.name);
            layer.visible = false;
        }
	}
}

function saveFolder(ref) {
	var len = ref.layers.length;
	for (var i = 0; i < len; i++) {
        var layer = ref.layers[i];
        if (layer.name == prefs.layersToExport) {
            // recurse if current layer is a group
            hideLayers(layer);
			saveLayers(layer);
        }
	}
}

function saveImage(layerName) {
	var handle = getUniqueName(prefs.filePath + "/" + baseFileName + layerName);
	prefs.count++;
    
    if(prefs.fileType=="PNG") {
        SavePNG(handle); 
	} else {
        SaveJPEG(handle); 
    }
}

function getUniqueName(fileroot) { 
    // form a full file name
	// if the file name exists, a numeric suffix will be added to disambiguate
	
    var filename = fileroot;
    for (var i=1; i<100; i++) {
        var handle = File(filename + "." + prefs.fileType); 
        if(handle.exists) {
            filename = fileroot + "-" + padder(i, 3);
        } else {
            return handle; 
        }
    }
} 

function padder(input, padLength) {
	// pad the input with zeroes up to indicated length
	var result = (new Array(padLength + 1 - input.toString().length)).join('0') + input;
	return result;
}

function SavePNG(saveFile) { 
    pngSaveOptions = new PNGSaveOptions(); 
	activeDocument.saveAs(saveFile, pngSaveOptions, true, Extension.LOWERCASE); 
} 

function SaveJPEG(saveFile) { 
    jpegSaveOptions = new JPEGSaveOptions(); 
	jpegSaveOptions.quality = prefs.fileQuality;
	activeDocument.saveAs(saveFile, jpegSaveOptions, true, Extension.LOWERCASE); 
} 

function Dialog() {
    // build dialogue
    var dlg = new Window ('dialog', 'Select Type'); 
	dlg.saver = dlg.add("dropdownlist", undefined, ""); 
	dlg.quality = dlg.add("dropdownlist", undefined, "");
	dlg.layersToExport = dlg.add("dropdownlist", undefined, "");
	
	
	for (var i = 0; i < activeDocument.layers.length; i++) {
		var layer = activeDocument.layers[i];
		if (layer.typename == 'LayerSet') {
		dlg.layersToExport.add ("item", layer.name, 0, {name: layer.name});
		}
	}
	dlg.layersToExport.add ("item", "All Layers", 0, {name: "All Layers"});

    // file type
    var saveOpt = [];
	saveOpt[0] = "PNG"; 
	saveOpt[1] = "JPG"; 
	for (var i=0, len=saveOpt.length; i<len; i++) {
		dlg.saver.add ("item", "Save as " + saveOpt[i]);
	}; 
	
    // trigger function
	dlg.saver.onChange = function() {
        prefs.fileType = saveOpt[parseInt(this.selection)]; 
		// turn on additional option for JPG
        if(prefs.fileType==saveOpt[1]){
            dlg.quality.show();
        } else {
            dlg.quality.hide();
        }
    }; 
	
	dlg.layersToExport.onChange = function() {
		prefs.layersToExport = (this.selection.properties.name);
	}
	  	   
	// jpg quality
    var qualityOpt = [];
	for(var i=12; i>=1; i--) {
        qualityOpt[i] = i;
        dlg.quality.add ('item', "" + i);
	}; 

    // trigger function
	dlg.quality.onChange = function() {
		prefs.fileQuality = qualityOpt[12-parseInt(this.selection)];
	};

    // remainder of UI
	var uiButtonRun = "Continue"; 

	dlg.btnRun = dlg.add("button", undefined, uiButtonRun ); 
	dlg.btnRun.onClick = function() {	
		this.parent.close(0); }; 

    dlg.orientation = 'column'; 

	dlg.saver.selection = dlg.saver.items[0];
	dlg.quality.selection = dlg.quality.items[0];
	dlg.layersToExport.selection = dlg.layersToExport.items[0];
	dlg.center(); 
	dlg.show();
}

function okDocument() {
     // check that we have a valid document
     
	if (!documents.length) return false;

	var thisDoc = app.activeDocument; 
	var fileExt = decodeURI(thisDoc.name).replace(/^.*\./,''); 
	return fileExt.toLowerCase() == 'psd'
}

function wrapper() {
	function showError(err) {
		alert(err + ': on line ' + err.line, 'Script Error', true);
	}

	try {
		// suspend history for CS3 or higher
		if (parseInt(version, 10) >= 10) {
			activeDocument.suspendHistory('Save Layers', 'main()');
		} else {
			main();
		}
	} catch(e) {
		// report errors unless the user cancelled
		if (e.number != 8007) showError(e);
	}
}

wrapper();