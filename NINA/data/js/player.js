var url_title ='http://' + location.host + '/toc';

function loadTitles() {
 fetch(url_title)
 .then(response => {return response.json();})
 .then(jsonOutput => {
	 
	 document.getElementById("titlelist").innerHTML = "";
	 for (var i in jsonOutput) {
		 document.getElementById("titlelist").innerHTML += '<a class="dropdown-item" href="/cmd.php?player:file=' + i + '">' + i + ' (' + jsonOutput[i] + ')</a>';
	 }
 })
 .catch(function() {
	// something went wrong
 });
}	

