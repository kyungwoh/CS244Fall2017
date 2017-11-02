<?php
header('Content-Type: application/excel');
header('Content-Disposition: attachment; filename="team11_assignment4.csv"');

$name = htmlspecialchars($_GET["name"]);

$conn = new mysqli("localhost", "root", "", "cs244") or die("Connect failed: %s\n". $conn -> error);
$stmt = $conn->prepare("SELECT SEQ,X,Y,Z FROM HW4 WHERE NAME = ? ORDER BY SEQ");
$stmt->bind_param("s",$name);
$stmt->execute();
$result = $stmt->get_result();

$fp = fopen('php://output','w');
$header = array('SEQ','X','Y','Z');
fputcsv($fp,$header);
while($row = $result->fetch_assoc()){
    fputcsv($fp,$row);
}
fclose($fp);
$conn->close();
?>