<?php
header('Content-Type: application/excel');
header('Content-Disposition: attachment; filename="hw6_input.csv"');

#$num = htmlspecialchars($_GET["num"]);

$conn = new mysqli("localhost", "root", "", "cs244") or die("Connect failed: %s\n". $conn -> error);
$stmt = $conn->prepare("SELECT NUM,SEQ,IR,RED,X,Y,Z FROM HW5 WHERE NUM > 0 ORDER BY NUM,SEQ");
#$stmt->bind_param("s",$num);
$stmt->execute();
$result = $stmt->get_result();

$fp = fopen('php://output','w');
$header = array('NUM','SEQ','IR','RED','X','Y','Z');
fputcsv($fp,$header);
while($row = $result->fetch_assoc()){
    fputcsv($fp,$row);
}
fclose($fp);
$conn->close();
?>