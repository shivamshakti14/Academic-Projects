<!DOCTYPE html>
<html>
<head>
	<title>web project.com</title>
	<link rel="stylesheet" type="text/css" href="css/style1.css">
	<link rel="stylesheet" href="css/style2.css">
	<link rel="stylesheet" href="css/style3.css">

</head>

<body >
		<h1 class="heading2">
			<p>
				&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
				&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
				&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
				&nbsp;&nbsp;
		
				No  &nbsp; Need &nbsp; To &nbsp;  Register &nbsp; or &nbsp;  Sign - Up.
				<br>
				<br>
				<br>
				<br>
				&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
				&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
				&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
				&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
				&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
				&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
				&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
				&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
				&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
				&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
				&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
				
				Easy &nbsp;&nbsp;| Simple &nbsp;&nbsp;| Fast
			</p>
		</h1>
		

<!--   now menu bar code begins....    -->

<label for="show-menu" class="show-menu">Show Menu</label>
	<input type="checkbox" id="show-menu" role="button">
		<ul id="menu">
		<li><a href="index.php">Home</a></li>
		<li>
			<a href="#">About Us</a>
			<ul class="hidden">
				<li><a href="about_us_who_we_are.php">Who We Are</a></li>
				<li><a href="about_us_what_we_do.php">What We Do</a></li>
			</ul>
		</li>
		<li>
			<a href="#">Check File Status </a>
			
		</li>
		<li>
			<a href="index2.php">Login /Sign-up</a>
		</li>
<br>

<br>
<br>
<br>
<br
<br>
<br>
<br>
<br>
<br>

<center>

<div class="boxed">
<br>
<br>
 <form>
<input class="MyButton1" type="button" value="Send Files" onclick="window.location.href='index.php'" />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
<input class="MyButton2" type="button" value="Recieve Files" onclick="window.location.href='recieve.php'" />
</form> 
<br>
<hr>
<br>
	<h2>
			
			<div class="browse2">
		<?php
    // Make sure an ID was passed
    if(isset($_POST['id'])) {
    // Get the ID
        $id = intval($_POST['id']);
		$yourpass=$_POST['pass2'];
		//echo "------$yourpass<br>";
        // Make sure the ID is in fact a valid ID
        if($id <= 0) {
            die('Entered ID is invalid!');
        }
        else {
            // Connect to the database
            $dbLink = new mysqli('localhost', 'root', '', 'dbtuts');
 			           
		   if(mysqli_connect_errno()) {
                die("MySQL connection failed: ". mysqli_connect_error());
           }
            // Fetch the file information
            $query = "
                SELECT `file`, `type`, `size`,`password`,`downloads`
                FROM `tbl_uploads`
                WHERE `id` = {$id}";
            $result = $dbLink->query($query);
       //$result = mysqli_querys($query);
     
            if($result) {
                // Make sure the result is valid
                if($result->num_rows == 1) {
                // Get the row
                    $row = mysqli_fetch_assoc($result);



  }

  else {
                    echo 'Wrong id or password';
                }
     
                // Free the mysqli resources
                @mysqli_free_result($result);
            }
            else {
                echo "Error! Query failed: <pre>{$dbLink->error}</pre>";
            }
            @mysqli_close($dbLink);
        }
    }
    else {
        echo 'Error! No ID was passed.';
    }

$path='http://localhost/series/final/uploads/'.$row['file'];
//$a=$row['file'];
//echo $path;
//echo '<h1><a href="$path">click here to get your file</a></h1>';
//echo "you entered=$_POST['pass2']";
if($yourpass == $row['password']){     

echo '<font color="red" size="25">File Found</font><br>';
echo '<br> <font color="green" ><b>File name  =</b></font>'.$row['file'];
echo '<br><br><font color="green" ><b>File type  =</b></font>'.$row['type'];
$lol=$row['size']/1024;
//echo '<br><br><font color="green" ><b>File size  =</b></font>'.$row['size'].' bytes';
echo '<br><br><font color="green" ><b>File size  =</b></font>'.$lol.' MB';
echo "<br><br><br><a href='".$path."'>Right Click here and<br>Click on Save link as </a>";

}
else{
	echo '<font color="red" size="25">Sorry wrong id or password</font>';
}

    ?>

	
	
	
			</div>
			<br>
			

		</form>
	</h2>
</div> 

	</center>
<br>


<br>

<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>



<hr>
<p class="soft">
© 2015 Indian Institute of Technology Guwahati : End-sem project by (Gaurav Anand , Shivam Shakti , Sandeep Kummar)
</p>
</body>
</html>