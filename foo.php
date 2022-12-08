<?php
$result = '<?xml version=\'1.0\' encoding=\'UTF-8\'?>\n<soap11env:Envelope xmlns:soap11env=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:tns=\"http://ws.nemsis.org/\"><soap11env:Body><tns:SubmitDataResponse><tns:requestType>SubmitData</tns:requestType><tns:requestHandle>7a882a2c-9777-4d77-8665-d431c587b112</tns:requestHandle><tns:statusCode>0</tns:statusCode></tns:SubmitDataResponse></soap11env:Body></soap11env:Envelope>';

$startHandle = strpos ( $result, '<tns:requestHandle>' );
								$endHandle = strpos ( $result, '</tns:requestHandle>' );
								$length = $endHandle - $startHandle - 19;
								
								$handle = substr ( $result, $startHandle + 19, $length );
echo $handle;
