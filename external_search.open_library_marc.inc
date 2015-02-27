<?php

function _cals_map_open_library_marcxml($item) {
	$xml = 
	'<marc:record>';
	/*
		<marc:leader>01649 a2200409 4500</marc:leader> 
		<marc:datafield tag="010" ind1=" " ind2=" "><marc:subfield code="a">87006910</marc:subfield></marc:datafield> 
		<marc:datafield tag="020" ind1=" " ind2=" "><marc:subfield code="a">0399132414</marc:subfield></marc:datafield>
		<marc:datafield tag="035" ind1=" " ind2=" "><marc:subfield code="a">M00584</marc:subfield></marc:datafield>
		<marc:datafield tag="040" ind1=" " ind2=" "><marc:subfield code="a">auvpabwa</marc:subfield></marc:datafield>
		<marc:datafield tag="082" ind1="0" ind2=" "><marc:subfield code="a">813/.54/583</marc:subfield></marc:datafield>
	*/		
	
	if(isset($item->oclc)) {
		foreach($item->oclc as $val) {
			$xml .= 
				'<marc:datafield tag="035" ind1=" " ind2=" "><marc:subfield code="a">(OCoLC)' . $val . '</marc:subfield></marc:datafield>';			}
	}
	
	foreach($item->author as $val) {
		$xml .=
		'<marc:datafield tag="100" ind1="1" ind2=" "><marc:subfield code="a">' . $val .'</marc:subfield></marc:datafield>';

	}


	$xml .= 	
		'<marc:datafield tag="245" ind1="1" ind2="4"><marc:subfield code="a">' . $item->title . '</marc:subfield><marc:subfield code="b"/><marc:subfield code="c">' . $item->author[0]. '</marc:subfield></marc:datafield>';

	if(isset($item->publish_place))	{

		foreach($item->publish_place as $val) $xml .=
			'<marc:datafield tag="264" ind1=" " ind2=" "><marc:subfield code="a">'. $val .'</marc:subfield></marc:datafield>';
	
	}
	if(isset($item->publisher))	{
		foreach($item->publisher as $val) $xml .= 
			'<marc:datafield tag="264" ind1=" " ind2=" "><marc:subfield code="b">' . $val . '</marc:subfield></marc:datafield>';
	}
	
	
	if(isset($item->publish_year)) {
		$xml .= 
			'<marc:datafield tag="264" ind1=" " ind2="0"><marc:subfield code="c">'. $item->publish_year.'</marc:subfield></marc:datafield>';

	}
	if(isset($item->description)) {
		$xml .= 
			'<marc:datafield tag="520" ind1=" " ind2=" "><marc:subfield code="a">' . $item->description .
				'</marc:subfield></marc:datafield>';
	}
	if(isset($item->isbn)) {
		if(!is_array($item->isbn)) $item->isbn = explode(", ", $item->isbn);
		foreach($item->isbn as $val)  $xml .=
				'<marc:datafield tag="534" ind1=" " ind2=" "><marc:subfield code="z">' . trim($val) .'</marc:subfield></marc:datafield>';
	
	}

	if(isset($item->subject)) {
		foreach($item->subject as $val) $xml .=		
			'<marc:datafield tag="650" ind1=" " ind2=" "><marc:subfield code="a">' . $val . '</marc:subfield></marc:datafield>';
	}
	if(isset($item->genre)) {
		foreach($item->genre as $val) $xml .=		
			'<marc:datafield tag="655" ind1=" " ind2=" "><marc:subfield code="a">' . $val . '</marc:subfield></marc:datafield>';
	}
	
	if(isset($item->contributor)) {
		foreach($item->contributor as $val)  $xml .= 
			'<marc:datafield tag="700" ind1="1" ind2=" "><marc:subfield code="a">'. $val .'</marc:subfield></marc:datafield>';

	}
	$xml .= '</marc:record>';
	return $xml;
}

/*

stdClass Object
(
    [edition_key] => Array
        (
            [0] => OL1575064M
            [1] => OL9685575M
            [2] => OL9798119M
            [3] => OL10943174M
        )


    [edition_count] => 4
    [key] => OL1955385W
 
 
    [id_librarything] => Array
        (
            [0] => 625644
        )
 



*/

	