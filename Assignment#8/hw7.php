<?php
$conn = new mysqli("localhost", "root", "", "cs244") or die("Connect failed: %s\n". $conn -> error);
//file_put_contents('f.dat', file_get_contents('php://input'));
//$fh = fopen ("f.dat", "rb");
$fh = fopen ("php://input", "rb");
while (!feof($fh)){
    $data = fread($fh, 4*5);
    $row = unpack("Iir/".
                  "Ired/".
                  "fx/".
                  "fy/".
                  "fz", $data);
    $NUM = '22';
    $IR = $row['ir'];
    $RED = $row['red'];
    $X = $row['x'];
    $Y = $row['y'];
    $Z = $row['z'];

    $stmt = $conn->prepare("INSERT INTO HW7(NUM,IR,RED,X,Y,Z) VALUES (?,?,?,?,?,?)");
    $stmt->bind_param("ssssss",$NUM,$IR,$RED,$X,$Y,$Z);
    $stmt->execute();
    $stmt->close();
}
fclose($fh);

$conn->close();
?>