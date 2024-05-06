    <?php  

	
	if (!isset($_SERVER["HTTP_HOST"])) {
		parse_str($argv[1], $_POST);
	}
	
	require($argv[2]);
     
    ?>  
