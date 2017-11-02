<?php
echo "Hello world<br>";
$conn = new mysqli("localhost", "root", "", "cs244") or die("Connect failed: %s\n". $conn -> error);
echo "Connection success<br>";
//error_log("connection ok");
$rawInput = file_get_contents('php://input');
error_log($rawInput);
//$rawInput = '{"data":[{"NUM":1,"SEQ":0,"IR":38624,"RED":8166},{"NUM":1,"SEQ":1,"IR":38587,"RED":8167}]}';
$obj = json_decode($rawInput,true);
foreach($obj['data'] as $row){
    $NAME = $row['NAME'];
    $SEQ = $row['SEQ'];
    $X = $row['X']; 
    $Y = $row['Y'];
    $Z = $row['Z'];
    $stmt = $conn->prepare("INSERT INTO HW4(NAME,SEQ,X,Y,Z) VALUES (?,?,?,?,?)");
    $stmt->bind_param("sssss",$NAME,$SEQ,$X,$Y,$Z);
    $stmt->execute();
    echo "INSERT success<br>";
    $stmt->close();
}
$conn->close();
echo "Bye";
?>