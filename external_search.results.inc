<?php


/**
 * Returns Search Results.
 *
 * @return
 *    HTML
 */ 

function external_search_view_new($searchtype = '', $keywords = '') {

  $rs = external_search_entries_load(1);  
  $results = ''; //array('#markup' => '');
  $lists = array();
  if ($rs->rowCount()) {
    foreach ($rs as $rses) {
      //build array from db object
      $rz = (array) $rses;
      $targets[] = $rz;
    }//foreach
    foreach($targets as $target) {
        $items = _cals_get_search_results_items ($keywords, $target);
        $lists[] = _external_search_multi_view ($items, $target, $keywords);
    }//foreach;
    $output = implode(' ', $lists);
    drupal_set_message(t('Please note: the links below will navigate you away from the NNELS website.'), 'status');
    return $output;
  }//if

  else {
    return t("No search service");
  }
}


function _external_search_multi_view($items, $target, $keywords)  {

        $urldec_keywords = urldecode($keywords);
        switch ($target['external_service']) {
            case 1:
            case "bookshare":
              $morelibrivox_link = url('https://www.bookshare.org/search', array(
                'query' => array(
                  'libraryToSearch' => 'bookshare',
                  'bookKeywords' => $urldec_keywords,
                  'booksToSearch' => 'FREELY_AVAILABLE',
                  'sortOrder' => 'RELEVANCE',
                  'disableSortOrder' => 'TITLE',
                  'search' => 'Advanced+Search',
                  '_categories' => 'on'
                  )));
              $items = array_slice($items, 0, 3); //grab the first 3 items from our list of filtered freelyAvailable
            break;

            case 2:
            case "libriVox":
              $more_link = url('https://librivox.org/search', array(
                'query' => array(
                  'q' => $urldec_keywords, 'search_form' => 'advanced', 'status' => 'complete')
                ));
            break;

            case 3:
            case "openlibrary":
              $more_link = url('https://openlibrary.org/search', array(
                'query' => array(
                  'q' => $keywords, 'has_fulltext' => 'true')  
                ));
            break;

            default:
            return FALSE;
            break;  
        }

        $case_service = ucfirst($target['external_service']);

        $theme_list = array();
        $themed_list .= '<div class="externalsearch-item-list">';
        $themed_list .= theme_item_list(array('items' => $items, 'title' => '', 
        'type' => "ul", 'attributes' => array("class"=>"search-results-list")));
        $themed_list .= '<div class="externalsearch-more-results">';

        $num_found = count($items);

        if(!empty($num_found)) {
          $themed_list .= t('Search full results at ') . l(t($case_service), $more_link, array(
          'attributes' => array(
            'class' => 'externalsearch-more-link')));
        }
        $themed_list .= '</div></div>';
        
        //no pager at this time; otherwise -> $pager = external_search_get_pager($num_found);
        
        $arr = array();
        $results = '<div><p>';
        if ($num_found) {
          $results .= 'Top ' .  number_format($num_found) . ' results found for <strong>"' .  $urldec_keywords . '"</strong> ' . 
           ' at ' . ucfirst($target['external_service'] . ':'); 
        }
        else {
          $results .=  t("Sorry, your search didn't return any records.");
        }
        // Format and print out items.
        $results .= '</p></div>';
        //$tmp = 'Your search has returned a total of ' . $num_found . ' records';
        $title = '<div class="externalsearch-number-of-hits">'. $results. '</div>' ;
        //$pager .
        return '<div class="target-results search-results">' . '<section><h2>'. $case_service . '</h2></section>' . $title . $themed_list .'</div>';
}

/**
 * Returns array of items.
 *
 * @return
 *    HTML
 * @todo
 *   tokenize the url string
 */ 

function _cals_get_search_results_items($keywords, $service) {
	//drupal_add_js(path_to_theme() . '/js/accordion.js', array('group' => JS_THEME));

	//$keywords = preg_replace('/\s+/', '+', $keywords); //should do this replace when creating the initial link

	$targets = array();
	$externalsearch = array();
	$externalsearch['keywords'] = $keywords;


	$url = token_replace($service['target_url'], $externalsearch); //@todo - figure out how to implement token_replace
	$url = str_replace("[keywords]", $keywords, $service['target_url']);
	$target[] = array(
		'targetid' => $service['targetid'],
		'url' => $url,
	);
  
  $items = _cals_extended_search_get_array_items($target);
	return $items;
}

/**
 * Wrapper function that returns array of items based on target service
 *
 * @return
 *    array
 */ 

function _cals_extended_search_get_array_items($target) {
  $options = array("absolute" => TRUE);
  $items = array();
  //loop thru each target and add to items array
  //foreach($targets as $target) {
    $targetid = $target[0]['targetid'];
    //basic string replacements
    $query = str_replace("  ", " ", $target[0]['url']);
    $query = str_replace(" ", "%20", $query);
    $query = trim($query);
    $results = '';
    $handle = fopen($query, "rb");
    $contents = stream_get_contents($handle);
    fclose($handle);
    $contents = json_decode($contents, TRUE);

    $records = array();
    
    switch($targetid) {
      case 1:
      case "bookshare":
        //$arr[$target] = $contents['bookshare']['book']['list']['result'];
        $records = $contents['bookshare']['book']['list']['result'];
      break;

      case 2:
      case "libriVox":
        $records = $contents['response']['docs'];
      break;

      case 3: 
      case "openlibrary":
        $records = $contents['docs'];
      break;

      default:
        return FALSE;
        break;  
    }
    $items = array_merge($items, _external_search_get_results($records, $target[0]));
  //} 
  return $items;

}


function _external_search_get_results($docs, $target) {
  $i=0;
  $items = array();
  foreach ($docs as $item) {
    $i++;
    switch($target['targetid']) {
      case 1:
      case "bookshare":
        $data = _external_search_bookshare_item_builder($item);
        break;

      case 2:
      case "libriVox":
        $data = _external_search_librivox_item_builder($item);
        break; 

      case 3:  
      case "openlibrary":
        $data = _external_search_openlibrary_item_builder($item);
        break;

    }
    $data = _external_search_format_result_item($data);
    
    if (!empty($data)) {
      $items[] = array(
        'data' => $data,          
        'class' => array("search-result"),
      );
    }
  }
  
  return $items;
}

/**
 * Returns a formatted field.
 *
 */
function _external_search_field_formatter($label, $fieldname, $value) {

  $label = str_replace("_", " ", $label);
  $html = '<div class="field-' . $fieldname . '">';
  if(strlen($label)) $html .= '<span class="field-label">' . t($label) .': </span>';
  $html .= '<div class="content">' . $value . '</div></div>';  
  return $html;
}

/**
 * Returns a formatted field.
 *
 */

function _external_search_format_result_item($item) {

  if (isset($item['title'])) { //Check if we have any entries blanked by Bookshare freelyAvailable == 0
  
    $rec = _external_search_field_formatter('', "title", $item['linked_title']);
    if(isset($item['contributor']))  $rec .= 
    _external_search_field_formatter('Contributor(s)', "contributor", $item['contributor']);

    $tmp = array();
    //ok this is a bit dirty - pull into a separate theme function?
    $authors = '';  //array();
    $label = t('Author');

    if(isset($item['author'])) {
      if (count($item['author']) > 1) $label = t('Authors');
      foreach($item['author'] as $author) {
        $tmp[] = $author;
      }
    }

    $attributes = array("class"=>"author-list");
    $authors = implode(", ", $tmp);
    $rec .= _external_search_field_formatter($label, "authors", $authors);

    $fields = array("publish_year", /*"publisher",*/ "isbn", "description" );
    foreach($fields as $field) {
      if(isset($item[$field])) $rec .= _external_search_field_formatter($field, $field, $item[$field]);
    }

  // if($item['target'] == 'bookshare') {
  // $create_node = l(t('Place an order request'), "request-new-title", array('query' => array('key' => $item['key'], "target" => "bookshare") ) );
  //}
  // else {
  //   $json = json_encode($item);
  //   $create_node = l(t('Place an order request'), "request-new-title", array('query' => array('vars' => $json) ) );
  //  }
  // $rec .= _external_search_field_formatter("", "link", $create_node);
  
  return $rec;
 }
}


//module_load_include('inc', 'calsauthen', '/drivers/' . $driver);

function _external_search_blurb_format($descr) {
  $descr = wordwrap(strip_tags($descr), 200);
  $descr = explode("\n", $descr);
  $descr = $descr[0] . '...';
  return $descr;
}

function _external_search_bookshare_item_builder($item) {
  //dpm($item['freelyAvailable'], 'Item was available? ');
  if ($item['freelyAvailable'] == 1) {

    $arr = array("publisher" => "publisher", "contributor" => "contributor");
    $me = array(
      'target' => "bookshare",
      'key' => $item['id'],
      'title' => $item['title'],
    
      'source' => "Bookshare",
      'url' => "http://bookshare.org/key/" . $item['id'],
      'author' => $item['author'],
      'linked_title' => l($item['title'], "http://bookshare.org/browse/book/" . $item['id']), 
    );
    if(isset($item['briefSynopsis'])) $me['description'] = _external_search_blurb_format($item['briefSynopsis']);
  
    foreach($arr as $k => $v) {
      if (array_key_exists($k, $item)) $me[$v] = $item[$k];
    } 
    return $me;
  }
    return FALSE;
}

function _external_search_openlibrary_item_builder($item) {

  $arr = array(
    //"author_name" => "author",  
    "oclc" => "oclc", 
    "publish_place" => "publish_place", 
    "language" => "language");
   // "isbn" => "isbn");
  
  $url = "http://openlibrary.org/" . $item['key'];
  $me = array(
    'target' => "openlibrary",
    'key' => $item['key'],
    'title' => $item['title'],
    
    'url' => $url, 
    'author' => $item['author_name'],
    'publish_year' => implode(", " , $item['publish_year']),
    'linked_title' => l($item['title'], $url),
  
  );
  //loop optional fields
  foreach($arr as $k => $v) {
    if (array_key_exists($k, $item)) $me[$v] = implode(", ", $item[$k]);
    
  } 
  return $me;

}

function _external_search_librivox_item_builder($item) {
  $arr = array('subject' => 'subject', 'publisher' => 'publisher');
  //http:\/\/
  $pattern = '/librivox.org\/(\w+(-\w+)*)(\w+(-\w+)*)*\/\\"/';
  preg_match($pattern, $item['description'], $matches);
  $url = "http://librivox.org/". $matches[1];

  $me = array (
    'target' => "libriVox",
    'key' => $item['identifier'],
    'title' => $item['title'],
    'author' => $item['creator'],
    'description' => _external_search_blurb_format($item['description']),
    'url' => $url,
    'publish_year' => $item['year'],
    'linked_title' => l($item['title'], $url),

  );
  //loop optional fields
  foreach($arr as $k => $v) {
    if (array_key_exists($k, $item)) $me[$v] = implode(", ", $item[$k]);
    
  } 
  return $me;

}


//($contents['hits'], $contents['hits_from'], $contents['hits_to'], $contents['next_page'])//, $from, $to, $next
function external_search_get_pager($total) {

  $page = pager_find_page();
  $num_per_page = 10;
  //$num_per_page = variable_get('mymodule_num_per_page', 10);
  $offset = $num_per_page * $page;
  //$result = mymodule_remote_search($keywords, $offset, $num_per_page);

  // Now that we have the total number of results, initialize the pager.
  pager_default_initialize($total, 10);
  
  // Display the search results.
  // display the pager controls, and return.
  
    $vars = array(
      'tags' => array(),
      'element' => 0,
      'parameters' => array(),
      'quantity' => 9,
    );
    $output = theme_pager($vars);
  //OLD ?? $output = theme('pager');
  return $output;
}


function _external_search_format_item($item,$id) {
    $snippet = '';
    $tmp = array("note" => "Notes", "source" => "Source", "subject" => "Subject(s)");

    foreach($tmp as $key => $label) {
      if(isset($item[$key])) {
        $items2 = array();
        foreach($item[$key] as $val) {
          $items2[] = array(
            'data' => '<p>' . $val . '</p>',
            'class' => array("result-details-item"),
          
          );
        }
        $attributes = array("class"=>"result-details-list");
        $snippet .= theme_item_list(array('items' => $items2, 'title' => $label, 'type' => "ul", 'attributes' => $attributes));
      }
    }

  $c = '<a db-nid="' . $id . '" href="#">more...</a>' .

        '<div class="db_toggle" db-nid="' . $id . '" style="display: none;">' . $snippet . '</div>';

  $c =  '<div class="result-detail-details" >' . $snippet . '</div>';

  $snippet = $c;    
  return $snippet;


}

