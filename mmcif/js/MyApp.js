// MyApp.js
$(document).ready(function () {
    $('.mytip').tooltip();

    var is_chrome = navigator.userAgent.toLowerCase().indexOf('chrome') > -1;
    var is_ff = navigator.userAgent.toLowerCase().indexOf('firefox') > -1;
    var user_agent = navigator.userAgent.toLowerCase()
    if (is_ff) {
	    $('iframe.my-iframe-handle').attr('width', '100%');
	    $('iframe.my-iframe-handle').attr('height', '800');
    } else if (is_chrome) {
	    $('iframe.my-iframe-handle').attr('width', '100%');
	    $('iframe.my-iframe-handle').attr('height', '800');        
    } else {
	//alert(user_agent);	
    }


});

