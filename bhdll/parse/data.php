<?
//401000
$dir="./dump/";
$files=scandir($dir);
$str="const char* PATCH_DATA=\"";

$original=file_get_contents("original.mem");
$brain=file_get_contents("brain.mem");

$except = 
	array
	(
			0x427b37 => 1
			,0x48663f => 1
			,0x48cf72 => 1
			,0x4f3368 => 1
	);

$pointer=4198400;

foreach($files as $file)
{
	if(!is_file($dir.$file))
		continue;
	echo $file;
	$str.="\"\r\n\t\"";


	$target=file_get_contents($dir.$file);

	$strModule = "";
	for($i=0;$i<strlen($file);$i++)
		$strModule.=$file[$i];
	$strModule.="\0";

	$len=strlen($original);
	unset($chk);
	unset($log);
	$log = array();
	for($i=0;$i<$len;$i++)
	{
		if(ord($original[$i])==ord($brain[$i]))
		if(ord($original[$i])!=ord($target[$i]))
		{
			if($chk[$i])
			{
				$chk[$i+1]=$chk[$i]+1;
				$log[$i-$chk[$i]]++;
			}
			else
			{
				$chk[$i+1]=1;
				$log[$i]=1;
			}
		}
	}
	echo "(".count($log).") : \n";
	foreach($log as $k=>$v)
	{
		if($except[$k + $pointer])
		{
			echo "(except) " . dechex($k+$pointer) . "\n";
			continue;
		}
		$strModule.=decTo4byteHex($k+$pointer);
		$strModule.=decTo4byteHex($v);
		echo dechex($k+$pointer)." : ".$v;
		echo " : ";
		for($i=0;$i<$v;$i++)
		{
			$strModule.=$original[$k+$i];
			echo dechex(ord($original[$k+$i])).",";
		}
		echo " : ";
		for($i=0;$i<$v;$i++)
		{
			$strModule.=$target[$k+$i];
			echo dechex(ord($target[$k+$i])).",";
		}
		echo "\n";
	}
	$strModule.="\0\0\0\0";
	for($i = 0; $i < strlen($strModule); $i++)
		$str .= "\\x".sprintf("%02x",ord($strModule[$i]) ^ 123);

//	preg_match_all("/< (\w+)    (\w+(?: \w+)?) /",$data,$mats,PREG_SET_ORDER);
//> 004FC210    B8 74A25100     MOV EAX,StarCraf.0051A274
}
function decTo4byteHex($in)
{
	return 
		 chr(($in&0xff000000) >> 24)
		.chr(($in&0x00ff0000) >> 16)
		.chr(($in&0x0000ff00) >> 8)
		.chr(($in&0x000000ff) >> 0);
	$in=dechex($in);
	$b=8-strlen($in);
	for($i=0;$i<$b;$i++)
		$in="0".$in;
//	return preg_replace("/\w\w/","\\x\\0",$in);
}
$strModule="\0\0\0\0";
for($i = 0; $i < strlen($strModule); $i++)
	$str .= "\\x".sprintf("%02x",ord($strModule[$i]) ^ 123);
$str.= "\";";

file_put_contents("patch.h",$str);
file_put_contents("../bhdll/patch.h",$str);
?>