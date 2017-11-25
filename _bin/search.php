
<html>

<head>
<title>Search</title>
</head>

<body>
<div align='center'><font color='#008800'><h2>Uahaaaaaaaaaa....</h2></font>
<form method="POST" action="search.php">
<h4><font color='#008800'>Uahaaaaaaa... the web <input type="text" name="search" size="20" value='<?php if(isset($_POST['search']))echo $_POST['search']?>'></font>
  <input type="submit" value="Submit" name="B1"></h4>
</form>
</div>
</body>
</html>

<?php
///////////////////////////////////////////////// YTOUR SETTINGS ////////////////////////////
define("EZSQL_DB_USER", "****");
define("EZSQL_DB_PASSWORD", "****");		
define("EZSQL_DB_HOST", "****");	    
define ("EZSQL_DB_NAME", "****");		
///////////////////////////////////////////////// YTOUR SETTINGS ////////////////////////////

require_once('@dbs.php');

if(isset($_POST['search']))
{
    Search($_POST['search']);
    die();
}
    
function Search(&$expresion)
{
    Prepare($expresion);
}

function Prepare(&$expresion)
{
    $n_words = count(explode(" ",$expresion));
    if (!get_magic_quotes_gpc()) 
    {
        $query_to_parse = addslashes($expresion);
    }
    else    
    {
        $query_to_parse = $expresion;
    }
    $my_query_string_link = stripslashes($query_to_parse);
    $query_to_parse = str_replace('_','\_',$query_to_parse); // avoid '_' in the query
    $query_to_parse = str_replace('%','\%',$query_to_parse); // avoid '%' in the query
    $query_to_parse = str_replace('\"',' ',$query_to_parse); // avoid '"' in the query
    $query_to_parse = strtolower($query_to_parse); 
    $query_to_parse = trim(ereg_replace(" +"," ",$query_to_parse)); // no more than 1 blank

    ////echo "<br>query to parse: ".$query_to_parse."<br />";
    
    DBSeach(explode(" ",$query_to_parse));

}
    
$sitename = null;
        
function DBSeach($words)
{
    global $D;
    $siteid = -1;
    $tables = null;
    $sqeq   = null;
    $ids    = array();
    $tbids  = array();
    $qry    = null;
    
    foreach ($words as $word)
    {
        if(!isset($ids[$word]))
        {
            $tbid       = _CalcCrc($word);
            
            if(!isset($tbids[$tbid]))
            {
                $tbids[$tbid] = $tbid;
                $ids[$word] = $tbid;
                
                if(strlen($qry))
                    $qry.="UNION ";
                $qry.="SELECT id,word FROM words{$tbid} WHERE word LIKE '%{$word}%' ";
            }
        }
    }
    $qry.=";";
    echo "<pre>{$qry}</pre>";
    if($rs = $D->get_results($qry))
    {
        $docs = array();
        foreach ( $rs as $r )
        {
            _ProcessWordIds($r->id, $r->word, $siteid, $docs);
        }
    }
}

// we have each word id then we have the word itself
// go and grab for each word the document list
function _ProcessWordIds($wordid, &$word, &$siteid, &$docs)
{
    global $D;
    $tbi = _CalcCrc($word);
    $qs  = "SELECT doc_tbl,doc_id,weight FROM search{$tbi} WHERE word_id={$wordid} ORDER BY weight DESC LIMIT 64;";
    if($rs = $D->get_results($qs))
    {
        echo "<table width='100%' border='0'><tr><td width='80%'><table border='0' width='100%'>";
        foreach ( $rs as $r )
        {
            _ProcessDocPages($r->doc_tbl, $r->doc_id, $r->weight, $word, $wordid, $siteid, &$docs);
        }
        echo "</table></td><td width='20%' bgcolor='F0F0F0' valign='top' align='center'>";
        echo "Sponsored Links";
        echo "</td></tr></table>";

    }
    
}

//
// select all documents
//
function _ProcessDocPages($doc_tbl, $doc_id, $weight, $word, $wordid, &$siteid, &$docs)
{
    global      $D;
    global      $sitename;
    
    $pagelink = null;

    $qs     = "SELECT filename,siteid FROM docs{$doc_tbl} WHERE id={$doc_id};";
    if($rs  = $D->get_results($qs))
    {
        foreach ( $rs as $r )
        {
            if($siteid != $r->siteid)
            {
                if($site = $D->get_results("SELECT name,service from sites{$doc_tbl} WHERE id={$r->siteid} LIMIT 1;"))
                {
                    $sitename  = $site[0]->name;
                    echo "<tr><td bgcolor='#ECECEC' colspan='2' align='right'><a href='http://{$sitename}' target='_blank'>{$sitename}</a></td></tr>\n";
                }
                $siteid = $r->siteid;
            }
            $docs[$siteid]['site'] = $siteid;

            $pagelink  = str_replace("C:/_WEBS/","http://",$r->filename);
            $pagelink  = str_replace("#0.htm","",$pagelink);
            
            echo "<tr><td><a href='{$pagelink}' target='_blank'>{$pagelink}</a></td><td align='right'>Relevance:{$weight}</td></tr>\n";
            echo "<tr><td colspan='2'>";
            _DumpFromReportFile($sitename, $r->filename, $word, $weight);
            echo "</td></tr>";
        }
    }
}

function _DumpFromReportFile(&$sitename, &$filename, &$word, $weight)
{
    $pos = strpos($filename, $sitename);
    
    $filesumary = substr($filename, 0, $pos + strlen($sitename)).'/'.$sitename.'.sum';
    
    $handle = fopen($filesumary,"rb");
    if($handle)
    {
        while (!feof($handle)) 
        {
            $buffer = fgets($handle, 256);
            if(strstr($buffer, $filename))
                break;
        }
        while (!feof($handle)) 
        {
            $buffer = fgets($handle, 256);
            $sp = strpos($buffer,  $word);
            if($sp)
            {
                echo substr($buffer,0,$sp);
                echo "<font color='#CC0000'>{$word}</font>";
                echo substr($buffer,$sp+strlen($word));
                break;
            }
        }
        fclose($handle);
    }

}
                            
    
  
function _CalcCrc(&$word)  
{
    $sum = 0;
    for($i=0; $i < strlen($word); $i++)
    {
        $sum += ord(substr($word,$i,1)); 
    }
    return $sum % 8;
}

?>