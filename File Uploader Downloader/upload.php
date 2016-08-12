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
	<h2>
			
	<br>
	<div class="browse">
<?php
include_once 'dbconfig.php';
if(isset($_POST['btn-upload']))
{    
     
	$file = rand(1000,100000)."-".$_FILES['file']['name'];
    $file_loc = $_FILES['file']['tmp_name'];
	$file_size = $_FILES['file']['size'];
	$file_type = $_FILES['file']['type'];
	$folder="uploads/";
	
	// new file size in KB
	$new_size = $file_size/1024;  
	// new file size in KB
	
	// make file name in lower case
	$new_file_name = strtolower($file);
	$naam=$new_file_name;
	$new_file_name = rand(100000,100000)."_".$new_file_name;
	
	// make file name in lower case
	
	$final_file=str_replace(' ','-',$new_file_name);
	$final_pass=0;	
	$final_pass=$_POST['pass'];
	if(move_uploaded_file($file_loc,$folder.$final_file))
	{
	$sql="INSERT INTO tbl_uploads(file,type,size,password) VALUES('$final_file','$file_type','$new_size','$final_pass')";
		mysql_query($sql);

/*echo <<<HTML
<a href="http://localhost/series/final/uploads/$final_file/">Click here to download</a>
the link is http://localhost/series/fileupload/uploads/$final_file/
HTML;
	*/

	echo '<font color="red">File upload Sucessfull<br></font>';
		echo '<br>';

		printf ("\nYour File  id =%d\n", mysql_insert_id());
echo '<br><br>';
$lol=($file_size);
echo "<font color='Grey' size='3.5'>File name=$naam <br>File size=$lol KB<br>File type=$file_type</font>";


		?>
		
		<?php
		
	
	}
	else
	{
		?>
		<script>
		alert('error while uploading file');
        window.location.href='index1.php?fail';
        </script>
		<?php
	}
}
?>



</div>			

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