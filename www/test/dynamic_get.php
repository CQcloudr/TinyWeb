<?php
   class Client {
       public $name = "";
       public $age  = "";
       public $function = "";
   }
   $e = new Client();
   $e->name = $_GET["name"];
   $e->age  =$_GET["age"];
   $e->function = "dynamic_get.php";
   echo json_encode($e);
?>
