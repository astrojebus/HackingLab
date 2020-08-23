<?php
session_start();    
    if(!isset($_SESSION['login'])) {
         echo "<h2>You must be logged in to access the hiddencontent page</h2>";
            
    }else {
echo '<h1><marquee width="1200"> u found the hidden page! congrats!!! </marquee>';
echo '<img src ="epic.gif"/>';
}
?>
