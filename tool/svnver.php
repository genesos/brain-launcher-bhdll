<?php
chdir(dirname(__FILE__));
chdir("../");
exec('"C:\\Program Files\\VisualSVN Server\\bin\\svn.exe" info -r HEAD --xml',$a);
preg_match("/revision=\"(\\d+)\"/",implode($a),$mat);
$str=<<<EOT
#pragma once
static wstring svnVersion=_T("{$mat[1]}");
EOT;
echo $str;
chdir(dirname(__FILE__));
$strOld=file_get_contents("svnver.h");
if($strOld!=$str)
	file_put_contents("svnver.h",$str);
//print_r($mat[1]);
?>