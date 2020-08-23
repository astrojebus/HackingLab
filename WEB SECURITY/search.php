<?php

 session_start();

    
    if(!isset($_SESSION['login'])) {
         echo "<h2>You must be logged in to access the search page</h2>";
    
    }else {


	echo '<i> lookup page views';
    $form = '<form><input type="text" name="q" id="q"></p><p><input type="Submit" value="search"></p></form>';
    echo $form;
    //SQLite3 :: escapeString is used to do a clean query of special characters
    $q = SQLite3::escapeString($_GET['q']);
    $con = new SQLite3("app.db");
    $query = "SELECT views FROM 'pages' WHERE php='$q'";
    $result = $con->query($query);
    $row = $result->fetchArray();
    if ($row){
		echo "page <b>$q</b>.php has <i>$row[0]</i> views";
    }
    else{
	//php's htmlentities function is used to clean up the parameters entered by users.
    	echo " this php page <b> " . (htmlentities($_GET['q'])) . " </b> does not exist!";
    }
}
?> 
