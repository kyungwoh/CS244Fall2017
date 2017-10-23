<?php
header('Content-Type: application/excel');
header('Content-Disposition: attachment; filename="team11_assignment2.csv"');

$conn = new mysqli("localhost", "root", "", "cs244") or die("Connect failed: %s\n". $conn -> error);
$sql = 'SELECT T1.SEQ, T1.IR IR1, T1.RED RED1, T2.IR IR2, T2.RED RED2, T3.IR IR3, T3.RED RED3, T4.IR IR4, T4.RED RED4 FROM (SELECT SEQ, IR, RED FROM HW2 WHERE NUM = 1) T1, (SELECT SEQ, IR, RED FROM HW2 WHERE NUM = 2) T2, (SELECT SEQ, IR, RED FROM HW2 WHERE NUM = 3) T3, (SELECT SEQ, IR, RED FROM HW2 WHERE NUM = 4) T4 WHERE T1.SEQ = T2.SEQ AND T1.SEQ = T3.SEQ AND T1.SEQ = T4.SEQ ORDER BY T1.SEQ';
$result = mysqli_query($conn,$sql);

$fp = fopen('php://output','w');
$header = array('SEQ','IR1','RED1','IR2','RED2','IR3','RED3','IR4','RED4');
fputcsv($fp,$header);

foreach($result as $line){
    fputcsv($fp,$line);
}
fclose($fp);

$conn->close();
?>