
<html>

<head>
<title>Search</title>
</head>
<body>
<div align='center'><font color='#008800'><h2>Uahaaaaaaaaaa....</h2></font>
<form method="POST" action="s.php">
<h4><font color='#008800'>Uahaaaaaaa... the web <input type="text" name="search" size="32" value='<?php if(isset($_POST['search']))echo $_POST['search'];?>'></font>
  <input type="submit" value="Submit" name="B1"></h4>
</form>
</div>
</body>
</html>

<?php
///////////////////////////////////////////////// YTOUR SETTINGS ////////////////////////////

define("PAGESIZE",16);
define("EZSQL_DB_USER", "****");
define("EZSQL_DB_PASSWORD", "****");		
define("EZSQL_DB_HOST", "localhost");	    
define ("EZSQL_DB_NAME", "****");		
///////////////////////////////////////////////// YTOUR SETTINGS ////////////////////////////

require_once('@dbs.php');

if(isset($_POST['search']))
{
    Prepare($_POST['search'],0,PAGESIZE);
}
else if(isset($_GET['f']))
{
    $search = null;
    for($w=0; $w < 8; $w++)
    {
        $key = 'w'.$w;
        if(isset($_GET[$key]))
        {
            $search .= " ".$_GET[$key];
        }
    }
    
    Prepare($search,$_GET['f'],$_GET['t']);
}
    

function Prepare(&$expresion,$from,$to)
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
    
    DBSeach(explode(" ",$query_to_parse),$from,$to);

}
    
$sitename = null;
        
function DBSeach($words,$from,$to)
{
    global $D;
    $siteid = -1;
    $tables = null;
    $sqeq   = null;
    $ids    = array();
    $tbids  = array();
    $qry    = null;

    $docs = array();
    
    $params = null;
    $wordcnt  = 1;
    foreach ($words as $word)
    {
        if(!isset($ids[$word]))
        {
            $tbid       = _CalcCrc($word);
            
            if(!isset($tbids[$tbid]))
            {
                $docs[$word] = array();
                $tbids[$tbid] = $tbid;
                $ids[$word] = $tbid;
                
                if(strlen($qry))
                    $qry.="UNION ";
                $qry.="SELECT id,word FROM words{$tbid} WHERE word='{$word}' ";
    
                $params.= "w".$wordcnt."=".$word."&";
                $wordcnt++;
            }
        }
    }
    $qry.=";";
    echo "<pre>{$qry}</pre><br />";
    if($rs = $D->get_results($qry)) //SELECT WORDS FROM SEARCH
    {
        foreach ( $rs as $r )
        {
            _ProcessWordIds($r->id, $r->word, $siteid, $docs,$from,$to);
        }

        $finalIDs  = array();
        $finaldocs = array();
        $sitenam   = array();

        while (list($key, $val) = each($docs)) 
        {
            ////echo "<b>{$key}</b><br />";                         // word
            if(is_array($val))
            {
                while (list($key2, $val2) = each($val)) 
                {
                    //echo "<b>{$key2}</b><br />";              // site
                    if(is_array($val2))
                    {
                        while (list($key3, $val3) = each($val2))  
                        {
                            //// echo "{$key3} = {$val3}<br />";

                             if(isset($finalIDs[$key3]))
                                $finalIDs[$key3] = $finalIDs[$key3]+1;
                            else
                                $finalIDs[$key3] = 1;
                
                            $finaldocs[$key3] = $val3;
                            $sitenam[$key3]   = $key2;
                        }
                    }
                }
            }
        }

        $cursite = null;

        echo "<table width='100%' border='0'><tr><td width='80%'><table border='0' width='100%'>";
        
        while (list($key, $val) = each($finalIDs))
        {
            $filename = $finaldocs[$key];
            $pagelink  = str_replace("C:/_WEBS/","http://",$filename);
            $pagelink  = str_replace("#0.htm","",$pagelink);
            $sname    = $sitenam[$key];
            
            if($cursite != $sname)    
            {
                $cursite = $sname;
                echo "<tr><td bgcolor='#ECECEC' align='center' colspan='2'><a href='http://{$cursite}' target='_blank'><b>{$cursite}</b></a> : Relevance: {$val}</td></tr>\n";
            }

            echo "<tr><td cospan='2'>";
            _DumpFromReportFile($sitenam[$key], $filename, $words, 0);
            echo "<br /> ";
            echo "<a href='{$pagelink}' target='_blank'>{$pagelink}</a>\n";
            echo "<br /></td></tr>";
        }

        echo "</table></td><td width='20%' valign='top' align='center'>";
        echo "Sponsored Links";
        
        
        $linknext = null;
        
        
        $prevfrom = $from - PAGESIZE;
        $prevto   = $to   - PAGESIZE;

        if($prevfrom<=0)
        {
            $prevfrom = 0;
            $prevto   = PAGESIZE;
        }
        else
            $linknext.= "<a href='s.php?f={$prevfrom}&t={$prevto}&{$params}'><<-Previous | </a>";
        
        if(count($finalIDs)>=PAGESIZE)
        {
            $nextfrom = $from + PAGESIZE;
            $nextto   = $to   + PAGESIZE;
            
            $linknext.= "<a href='s.php?f={$nextfrom}&t={$nextto}&{$params}'> | Next->></a>";
        }
        
        echo "</td></tr><tr><td bgcolor='#CCCCCC' align='center'>{$linknext}</tr></td></table>";

    }
}

// we have each word id then we have the word itself------------------------------------------
// go and grab for each word the document list
function _ProcessWordIds($wordid, &$word, &$siteid, &$docs,$from,$to)
{
    global $D;
    $tbi = _CalcCrc($word);
    $qs  = "SELECT doc_tbl,doc_id,weight FROM search{$tbi} WHERE word_id={$wordid} ORDER BY weight DESC LIMIT {$from},{$to};";
    if($rs = $D->get_results($qs))
    {
        foreach ( $rs as $r )
        {
            _ProcessDocPages($r->doc_tbl, $r->doc_id, $r->weight, $word, $wordid, $siteid, $docs);
        }
    }
}

//
// select all documents----------------------------------------------------------------------
//
function _ProcessDocPages($doc_tbl, $doc_id, $weight, $word, $wordid, &$siteid, &$docs)
{
    global      $D;
    global      $sitename;
    
    $pagelink = null;

    $qs     = "SELECT filename,siteid FROM docs{$doc_tbl} WHERE id={$doc_id};";
    if($rs  = $D->get_results($qs))
    {
        $counter = 0;
        foreach ( $rs as $r )
        {
            if($siteid != $r->siteid)
            {
                if($site = $D->get_results("SELECT name,service from sites{$doc_tbl} WHERE id={$r->siteid} LIMIT 1;"))
                {
                    $sitename  = $site[0]->name;
                }
                $siteid = $r->siteid;
            }
            
            $docs[$word][$sitename][$doc_id] = $r->filename;
        }
    }
    
}

function _DumpFromReportFile(&$sitename, &$filename, &$words, $weight)
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
            $breakhere=0;

            foreach ($words as $word)
            {
                $sp = strpos($buffer,  $word);
                if($sp)
                {
                    echo substr($buffer,0,$sp);
                    echo "<font color='#CC0000'>{$word}</font>";
                    echo substr($buffer,$sp+strlen($word));
                    $breakhere=1;
                    break;
                }
            }
            if($breakhere)
                break;
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
