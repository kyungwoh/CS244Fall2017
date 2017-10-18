<?php
echo "Hello world<br>";

$input = $_GET["input"];

echo $input;
echo "<br>";

$conn = new mysqli("localhost", "root", "", "cs244") or die("Connect failed: %s\n". $conn -> error);
$stmt = $conn->prepare("INSERT INTO HW1(COL1) VALUES (?)");
$stmt->bind_param("s",$input);
$stmt->execute();
$stmt->close();
$conn -> close();

echo "Bye";
?>