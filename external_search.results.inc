<?php


/**
 * Returns Search Results.
 *
 * @return
 *    HTML
 */ 

function external_search_view_new($searchtype = '', $keywords = '') {

  $rs = external_search_entries_load(1, 0);  //$status = 1, active
  $results = ''; //array('#markup' => '');
  $lists = array();

  if ($rs->rowCount()) {
    foreach ($rs as $rses) {
      //build array from db object
      $rz = (array) $rses;
      $targets[] = $rz;
    }

    foreach($targets as $target) {
        $items = external_search_get_results_items($keywords, $target);
        $lists[] = external_search_multi_view($items, $target, $keywords);
    }

    $output = implode(' ', $lists);

    drupal_set_message(t('Please note: the links in the sidebar as well as below will navigate you away from the NNELS website.'), 'status');
    
    return $output;

  }//if

  else {
    
    return t("No search service");
  
  }

}

function external_search_single_view ($target_slug = '', $keywords = '') {
   if ($target_slug === 'bccampus') $targetid = 11;

   switch ($target_slug) {
    case 'bccampus':
      $targetid = 11;
    break;

    case 'openlibrary':
      $targetid = 8;
    break;

    case 'internet-archive':
      $targetid = 13;
    break;
   }

   $rs = external_search_load_by_targetid( $targetid );

   if ($rs->rowCount() ) {
    foreach ( $rs as $rses ) {
      $qual_target = (array) $rses;
   }
 }

   $items = external_search_get_results_items($keywords, $qual_target);
   $lists[] = external_search_multi_view($items, $qual_target, $keywords);

   $output = implode(' ', $lists);
   return $output;
}

function external_search_multi_view($items, $target, $keywords)  {

        //filter special chars from our queries that might interfere with target API or be stripped anyway, except + plus
        $filter_keywords = preg_replace('/[^a-zA-Z0-9_ +%\[\]\.\(\)%&-]/s', '', $keywords);
        
        switch ($target['external_service']) {
            case 2:
            case "bookshare":
              $more_link = url('https://www.bookshare.org/search', array(
                'query' => array(
                  'libraryToSearch' => 'bookshare',
                  'bookKeywords' => $filter_keywords,
                  'booksToSearch' => 'FREELY_AVAILABLE',
                  'sortOrder' => 'RELEVANCE',
                  'disableSortOrder' => 'TITLE',
                  'search' => 'Advanced+Search',
                  '_categories' => 'on'
                  )));

              $items = array_slice($items, 0, 3); //grab the first 3 items from our list of filtered freelyAvailable
            break;

            case 5:
            case "libriVox":
              $more_link = url('https://librivox.org/search', array(
                'query' => array(
                  'q' => $filter_keywords, 'search_form' => 'advanced', 'status' => 'complete'
                  )
                ));
            break;

            case 8:
            case "openlibrary":
              $more_link = url('https://openlibrary.org/search', array(
                'query' => array(
                  'q' => $filter_keywords, 'has_fulltext' => 'true'
                  )  
                ));
            break;

            case 11:
            case "BCcampus OpenEd":
              $more_link = url('http://open.bccampus.ca/find-open-textbooks/', array(
                'query' => array(
                  'searchTerm' => $filter_keywords 
                  ),
                ));
            break;

            case 12:
            case "Project Gutenberg":
              $more_link = url('https://www.gutenberg.org/ebooks/search/', array(
                'query' => array(
                  'query' => $filter_keywords
                  ),
                ));
            break;

            case 13:
            case "IA Audiobooks & Poetry":
              $more_link = url('https://archive.org/search.php', array(
                'query' => array(
                  'query' => '('.$filter_keywords.')' . ' AND -collection:(librivoxaudio) -collection:(gutenberg) collection:(audio_bookspoetry) mediatype:(audio)'
                  ),
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
        $themed_list .= '<div class="externalsearch-more-results"><span class="more-results">';

        $num_found = count($items);

        if(!empty($num_found)) {
          $themed_list .= t('Search full results at ') . l(t($case_service), $more_link, array(
          'attributes' => array(
            'class' => 'externalsearch-more-link')));
        }
        $themed_list .= '</span></div></div>';
        
        //no pager at this time; otherwise -> $pager = external_search_get_pager($num_found);
        
        $arr = array();
        $results = '<div><p>';
        if ($num_found) {
          $results .= 'Top ' .  number_format($num_found) . ' results found for <strong>"' .  $keywords . '"</strong> ' . 
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

function external_search_get_results_items($keywords, $service) {

	$targets = array();
	$externalsearch = array();
	$externalsearch['keywords'] = $keywords;


	$url = token_replace($service['target_url'], $externalsearch); //@todo - figure out how to implement token_replace
	$url = str_replace("[keywords]", $keywords, $service['target_url']);

	$target[] = array(
		'targetid' => $service['targetid'],
		'url' => $url,
	);
  
  $items = external_search_get_array_items($target);
	return $items;
}

/**
 * Wrapper function that returns array of items based on target service
 *
 * @return
 *    array
 */ 

function external_search_get_array_items($target) {
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
      case 2:
        //$arr[$target] = $contents['bookshare']['book']['list']['result'];
        $records = $contents['bookshare']['book']['list']['result'];
      break;

      case 8: 
        $records = $contents['docs'];
      break;

      case 11:
        $records = $contents['results'];
      break;

      case 5:
      case 12:
      case 13:
        $records = $contents['response']['docs'];
      break;

      default:
        return FALSE;
        break;  
    }
    $items = array_merge($items, external_search_get_results($records, $target[0]));
  //}
  return $items;

}


function external_search_get_results($docs, $target) {
  $i=0;
  $items = array();
  foreach ($docs as $item) {
    $i++;
    switch($target['targetid']) {
      case 2:
        $data = external_search_bookshare_item_builder($item);
        break;

      case 5:
        $data = external_search_librivox_item_builder($item);
        break; 

      case 8:  
        $data = external_search_openlibrary_item_builder($item);
        break;

      case 11:
        $data = external_search_bccampus_item_builder($item);
        break;

      case 12:
        $data = external_search_gutenberg_item_builder($item);
        break;

      case 13:
        $data = external_search_archive_item_builder($item);
        break;

    }
    $data = external_search_format_result_item($data);
    
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
function external_search_field_formatter($label, $fieldname, $value) {

  $label = str_replace("_", " ", $label);
  $html = '<div class="field-' . $fieldname . '">';
  if(strlen($label)) $html .= '<span class="field-label">' . t( ucfirst($label) ) .': </span>';
  $html .= '<div class="content">' . $value . '</div></div>';  
  return $html;
}

/**
 * Returns a formatted field.
 *
 */

function external_search_format_result_item($item) {

  if (isset($item['title'])) { //Check if we have any entries blanked by Bookshare freelyAvailable == 0
  
    $rec = external_search_field_formatter('', "title", $item['linked_title']);
    if(isset($item['contributor']))  $rec .= 
    external_search_field_formatter('Contributor(s)', "contributor", $item['contributor']);

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
    $rec .= external_search_field_formatter($label, "authors", $authors);

    $fields = array("publish_year", "formats", "isbn", "description" );
    foreach($fields as $field) {
      if(isset($item[$field])) $rec .= external_search_field_formatter($field, $field, $item[$field]);
    }

  // if($item['target'] == 'bookshare') {
  // $create_node = l(t('Place an order request'), "request-new-title", array('query' => array('key' => $item['key'], "target" => "bookshare") ) );
  //}
  // else {
  //   $json = json_encode($item);
  //   $create_node = l(t('Place an order request'), "request-new-title", array('query' => array('vars' => $json) ) );
  //  }
  // $rec .= external_search_field_formatter("", "link", $create_node);
  
  return $rec;
 }
}


//module_load_include('inc', 'calsauthen', '/drivers/' . $driver);

function external_search_blurb_format($descr) {
  $descr = wordwrap(strip_tags($descr), 200);
  $descr = explode("\n", $descr);
  $descr = $descr[0] . '...';
  return $descr;
}

function external_search_bookshare_item_builder($item) {
  //dpm($item['freelyAvailable'], 'Item was available? ');

  if ($item['freelyAvailable'] == 1) {

    $formats = rtrim(implode(', ', $item['downloadFormat']), ', ');
    
    $arr = array("publisher" => "publisher", "contributor" => "contributor");
    $me = array(
      'target' => "bookshare",
      'key' => $item['id'],
      'title' => $item['title'],
      'formats' => $formats,
      'source' => "Bookshare",
      'url' => "http://bookshare.org/key/" . $item['id'],
      'author' => $item['author'],
      'linked_title' => l($item['title'], "http://bookshare.org/browse/book/" . $item['id']), 
    );
    if(isset($item['briefSynopsis'])) $me['description'] = external_search_blurb_format($item['briefSynopsis']);
  
    foreach($arr as $k => $v) {
      if (array_key_exists($k, $item)) $me[$v] = $item[$k];
    } 
    return $me;
  }
    return FALSE;
}

function external_search_openlibrary_item_builder($item) {

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

function external_search_librivox_item_builder($item) {
  $arr = array('subject' => 'subject', 'publisher' => 'publisher');
  //http:\/\/
  $pattern = '/librivox.org\/(\w+(-\w+)*)(\w+(-\w+)*)*\/\\"/';
  preg_match($pattern, $item['description'], $matches);

  $url = "http://librivox.org/". $matches[1];

  $formats = rtrim(implode(', ', $item['format']), ', ');

  $me = array(
    'target' => "libriVox",
    'key' => $item['identifier'],
    'title' => $item['title'],
    'author' => $item['creator'],
    'formats' => 'Various',
    'description' => external_search_blurb_format($item['description']),
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

function external_search_gutenberg_item_builder($item) {
  $call_number = filter_var($item['call_number'], FILTER_SANITIZE_NUMBER_INT);
  $url = 'https://www.gutenberg.org/ebooks/' . $call_number;

  $formats = rtrim(implode(', ', $item['format']), ', ');

  $me = array(
      'target' => 'Project Gutenberg',
      'key' => $item['identifier'],
      'title' => $item['title'],
      'author' => $item['creator'],
      'formats' => $formats,
      'description' => external_search_blurb_format($item['description']),
      'linked_title' => l($item['title'], $url),
  );

  return $me;

}

function external_search_archive_item_builder($item) {
  $identifier = $item['identifier'];
  $url = 'https://archive.org/details/' . $identifier;

  //$formats = rtrim(implode(', ', $item['format']), ', ');

  $me = array(
      'target' => 'IA Audiobooks & Poetry',
      'key' => $item['identifier'],
      'title' => $item['title'],
      'author' => $item['creator'],
      'formats' => 'Various',
      'description' => external_search_blurb_format($item['description']),
      'linked_title' => l($item['title'], $url),
  );

  return $me;

}

function external_search_bccampus_item_builder($item) {

  $url = 'http://open.bccampus.ca/find-open-textbooks/?uuid=' . $item['uuid'];
  $me = array(
    'target' => 'BCcampus OpenEd',
    'key' => $item['uuid'],
    'title' => $item['name'],
    'description' => external_search_blurb_format($item['description']),
    //'publish_year' => implode(", " , $item['publish_year']),
    'linked_title' => l($item['name'], $url),
  );

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


function external_search_format_item($item,$id) {
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

