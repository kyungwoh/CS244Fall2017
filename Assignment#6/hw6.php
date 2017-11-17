<?php
echo "Hello world<br>";
$conn = new mysqli("localhost", "root", "", "cs244") or die("Connect failed: %s\n". $conn -> error);
echo "Connection success<br>";
$rawInput = file_get_contents('php://input');
#$rawInput = '{"data":[[1,950,119586,91336,0.00,-0.06,0.98],[1,951,119620,91306,0.00,-0.05,0.98],[1,952,119580,91383,0.00,-0.05,0.99],[1,953,119518,91280,0.00,-0.05,0.98],[1,954,119370,91297,0.00,-0.05,0.99],[1,955,119414,91231,0.01,-0.05,0.99],[1,956,119374,91286,0.01,-0.05,0.98],[1,957,119401,91255,0.01,-0.06,0.98],[1,958,119398,91279,0.01,-0.06,0.98],[1,959,119436,91320,0.00,-0.05,0.98],[1,960,119422,91218,0.01,-0.06,0.98],[1,961,119359,91277,0.00,-0.06,0.98],[1,962,119432,91289,0.00,-0.05,0.98],[1,963,119454,91296,0.00,-0.06,0.99],[1,964,119477,91262,0.01,-0.06,0.98],[1,965,119472,91299,0.01,-0.05,0.99],[1,966,119452,91286,0.01,-0.05,0.99],[1,967,119470,91307,0.01,-0.05,0.98],[1,968,119457,91285,0.00,-0.06,0.99],[1,969,119487,91304,0.01,-0.06,0.99],[1,970,119479,91271,0.01,-0.06,0.99],[1,971,119526,91260,0.01,-0.06,0.98],[1,972,119462,91201,0.01,-0.06,0.99],[1,973,119299,91213,0.00,-0.05,0.99],[1,974,119281,91211,0.00,-0.05,0.99],[1,975,119287,91208,0.01,-0.06,0.98],[1,976,119255,91202,0.00,-0.06,0.99],[1,977,119283,91194,0.00,-0.06,0.98],[1,978,119268,91228,0.00,-0.05,0.99],[1,979,119280,91159,0.01,-0.05,0.99],[1,980,119226,91190,0.00,-0.06,0.98],[1,981,119275,91128,0.00,-0.05,0.98],[1,982,119222,91172,0.01,-0.06,0.98],[1,983,119296,91177,0.00,-0.06,0.98],[1,984,119292,91219,0.00,-0.05,0.98],[1,985,119347,91133,0.00,-0.05,0.99],[1,986,119272,91192,0.00,-0.06,0.99],[1,987,119320,91141,0.01,-0.06,0.99],[1,988,119310,91140,0.01,-0.05,0.99],[1,989,119298,91190,0.01,-0.05,0.99],[1,990,119431,91219,0.01,-0.06,0.98],[1,991,119482,91223,0.00,-0.06,0.98],[1,992,119356,91178,0.00,-0.06,0.98],[1,993,119178,91102,0.00,-0.06,0.98],[1,994,119108,91185,-0.01,-0.05,0.97],[1,995,119372,91357,0.01,-0.05,0.98],[1,996,120049,92150,0.01,-0.06,0.99],[1,997,122148,93545,0.01,-0.06,0.98],[1,998,124654,94649,-0.00,-0.05,0.96],[1,999,125384,90853,0.01,-0.09,0.95]]}';
#error_log($rawInput);
$obj = json_decode($rawInput,true);
foreach($obj['data'] as $row){
    $NUM = $row[0];
    $SEQ = $row[1];
    $IR = $row[2];
    $RED = $row[3];
    $X = $row[4];
    $Y = $row[5];
    $Z = $row[6];
    $stmt = $conn->prepare("INSERT INTO HW5(NUM,SEQ,IR,RED,X,Y,Z) VALUES (?,?,?,?,?,?,?)");
    $stmt->bind_param("sssssss",$NUM,$SEQ,$IR,$RED,$X,$Y,$Z);
    $stmt->execute();
    $stmt->close();
}
$conn->close();
echo "Bye";
?>