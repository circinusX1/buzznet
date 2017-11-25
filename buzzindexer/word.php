<div align='center'><font color='#008800'><h2>Uaha</h2></font>
<form method="POST" action="word.php">
<h4><font color='#008800'>Search
<input type="text" name="search" size="32" value='
<?php if(isset($_POST['search']))echo $_POST['search'];?>'></font>
 <input type="submit" value="Submit" name="B1"></h4>
</form>
</div>
<div align='center'>


<?php
// localhost,buzznet,marius,elamar
define ("SITE_TABLES",16) ;
define ("WORD_TABLES",16*6);
define("PAGESIZE",8);
define("EZSQL_DB_USER", "marius");
define("EZSQL_DB_PASSWORD", "elamar");		
define("EZSQL_DB_HOST", "localhost");	    
define ("EZSQL_DB_NAME", "buzznet");		

require_once('@dbs.php');
$QRYQ = null;

if(isset($_POST['search']))
{
    Prepare($_POST['search'],0,PAGESIZE);
}

function Prepare(&$expresion,$from,$to)
{
    global $D;
    $tbid       = _CalcCrc($expresion);
    $qry="SELECT id,word FROM words{$tbid} WHERE word ='".$expresion."';";

    echo $qry."<br />";

    echo $tbid."<br />";
    $QRYQ =$qry;
    if($rs = $D->get_results($qry)) //SELECT WORDS FROM SEARCH
    {
        foreach ( $rs as $r )
        {
            echo $r->id."<br />";
            
            
           $qry2 = "SELECT doc_tbl,doc_id,weight FROM search{$tbi}";
           $qry2 .= "WHERE word_id={$r->id} ORDER BY weight DESC;
           
           if($rs = $D->get_results($qry2))
           {
                foreach ( $rs as $r )
                {
                     echo "tabela: {$r->doc_tbl} - doc: {$r->doc_id} <br />";
                }
           }
            break;
        }
    }
}

function _CalcCrc(&$word)
{
    $sum = 0;
    for($i=0; $i < strlen($word); $i++)
    {
        $wrd = ord(substr($word,$i,1))-20;
        if($wrd<0)$wrd00;
        $sum = ($sum * 31) + $wrd ;
        $sum &= 0xFFFFFFFF;
    }
    $sum &= 0x7FFFFFFF;
    return $sum % WORD_TABLES;
}

?>
