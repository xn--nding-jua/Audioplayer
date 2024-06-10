// prototypes for charts and timeseries
var chart_volumes;

var timeseries=[];
timeseries["timeseries_left"] = new TimeSeries();
timeseries["timeseries_right"] = new TimeSeries();
timeseries["timeseries_sub"] = new TimeSeries();

function createChartTimelines() {
	// create timeseries
	chart_volumes = new SmoothieChart({millisPerPixel:100,
		grid:{strokeStyle:'rgba(119,119,119,0.5)',verticalSections:4},tooltip:true,maxValue:100,minValue:0,
		horizontalLines:[{color:'#f0f0f0',lineWidth:1,value:0},{color:'#f0f0f0',lineWidth:1,value:25},{color:'#f0f0f0',lineWidth:1,value:75}]});
	
	chart_volumes.addTimeSeries(timeseries["timeseries_left"], { strokeStyle: 'rgba(0, 255, 0, 1)', lineWidth: 4 });
	chart_volumes.addTimeSeries(timeseries["timeseries_right"], { strokeStyle: 'rgba(255, 0, 0, 1)', lineWidth: 4 });
	chart_volumes.addTimeSeries(timeseries["timeseries_sub"], { strokeStyle: 'rgba(0, 0, 255, 1)', lineWidth: 4 });
	
	chart_volumes.streamTo(document.getElementById("chart_volumes"), 500);
}

function changeScopeSpeed(speed) {
	chart_volumes.options.millisPerPixel=speed;
}
