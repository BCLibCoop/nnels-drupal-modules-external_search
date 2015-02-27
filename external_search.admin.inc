<?php


/**
 * Implements module settings.
 *
 */
function external_search_admin_settings() {
 
  $targets = external_search_list_targets();
  $form['authentication_targets'] = array(
      '#prefix' => '<div>',
      '#markup' => $targets,
      '#suffix' => '</div>',
      '#weight' => -20,
  );
	$form1 = system_settings_form($form);
 	//$add_form = system_settings_form(external_search_add_target());
 	
  //return $add_form . system_settings_form($form);
  return $form1; // . $form2;

}


function _external_search_get_edit_form(&$form) {


}

/**
* Define a form.
*/
function external_search_add_target() {

	$form['current_targets'] = array(
      '#markup' => external_search_list_targets(),
   );
	$form['overview'] = array(
      '#markup' => t('Add new service here:'),
   ); 
  $form['external_service'] = array(
    '#title' => t('External Database Service'),
    '#type' => 'textfield',
    '#size' => 20,
    '#required' => TRUE, 
    '#description' => t('Name of the external database / catalog'),
    '#prefix' => '<div class="block-content clearfix">',
   );
  $form['api_key'] = array(
    '#title' => t('API Key'),
    '#type' => 'textfield',
    '#size' => 20,
    '#required' => TRUE, 
    '#description' => t('The API key used in searches, etc.'),
 
  );
  $form['status'] = array(
    '#title' => t('Current status'),
    '#type' => 'radios',
    '#size' => 2,
    '#required' => TRUE, 
    '#options' => array( 0 => t("Disabled"), 1 => t("Active target") ),
    '#default_value' => 0,
  );
  $form['weight'] = array(
    '#title' => t('Relative weight on search page'),
    '#type' => 'textfield',
    '#size' => 2,
    '#required' => TRUE, 
    '#default_value' => 88,
    '#description' => t("lower weights will display higher"),
  );

  $form['target_url'] = array(
    '#title' => t('Target URL'),
    '#type' => 'textfield',
    '#size' => 40,
    '#maxlength' => 300,
    '#required' => TRUE, 
    '#default_value' => '',
    '#attributes' =>array('placeholder' => t('http://targeturl.com/myapi_key/[keywords]')),
    '#description' => t('Add the target URL, including tokens and api key'),
  );



  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Add External Target'),
    '#suffix' => '</div>',
  );

  return $form;
}



function external_search_edit_target($form, &$form_state, $targetid) {

	$args = func_get_args();
	
	$rs = db_query("SELECT * FROM {external_search_targets} WHERE targetid = :id limit 1", array(':id' => $targetid));
	//$rs->fetchAllAssoc();
 
  if ($rs->rowCount()) {
		$values = array();
		while ($row = $rs->fetchAssoc()) {
			$values = $row;
		} 
  }
  else {
    $form['warning'] = array(
      '#markup' => t('<div class="messages warning">No search service target has been set for this value. <a href="@here">You can define one here</a>.</div>',
        array('@here' => url('admin/config/search/external-search'))),
    );
    return $form;
  }

	
	$form['targetid'] = array(
	  '#type' => 'value', '#value' => $values['targetid'],
	);
  $form['external_service'] = array(
    '#title' => t('External Database Services?'),
    '#type' => 'textfield',
    '#size' => 20,
    '#required' => TRUE, 
    '#description' => t('Name of the external database / catalog'),
    '#default_value' => $values['external_service'],
    //'#prefix' => '<div class="block-content clearfix">',
   );
  $form['api_key'] = array(
    '#title' => t('API Key'),
    '#type' => 'textfield',
    '#size' => 20,
    '#required' => TRUE, 
    '#description' => t('The API key used in searches, etc.'),
    '#default_value' => $values['api_key'],
 
  );

  $form['status'] = array(
    '#title' => t('Current status'),
    '#type' => 'radios',
    '#size' => 2,
    '#required' => TRUE, 
    '#options' => array( 0 => t("Disabled"), 1 => t("Active target") ),
    '#default_value' => $values['status'],
  );
  $form['weight'] = array(
    '#title' => t('Relative weight on search page'),
    '#type' => 'textfield',
    '#size' => 2,
    '#required' => TRUE, 
    '#default_value' => 88,
    '#description' => t("lower weights will display higher"),
  );

  $form['target_url'] = array(
    '#title' => t('Target URL'),
    '#type' => 'textfield',
    '#size' => 40,
    '#maxlength' => 300,
    '#required' => TRUE, 
    '#default_value' => $values['target_url'],
    '#attributes' =>array('placeholder' => t('http://targeturl.com/myapi_key/[keywords]')),
    '#description' => t('Add the target URL, including tokens and api key'),
  );



  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Update Target'),
    '#suffix' => '</div>',
  );
 
  return $form;
}

function external_search_delete_confirm_target($form, &$form_state, $targetid) {
	$rs = db_query("SELECT * FROM {external_search_targets} WHERE targetid = :id", array(':id' => $targetid ) )->fetchAssoc();

  $form['targetid'] = array('#type' => 'value', '#value' => $targetid);
  $form['external_service'] = array('#type' => 'value', '#value' => $rs['external_service']);

  $form['delete'] = array(
    '#type' => 'submit',
    '#value' => t('Delete target')
  );
  $form['cancel'] = array(
    '#type' => 'submit',
    '#value' => t('Cancel')
  );
  $form['warning'] = array(
    '#markup' => t('<div class="messages warning">Do you want to delete the authentication settings for %title? This cannot be undone.</div>', array('%title' => $rs['external_service'])),
    '#weight' => -50
  );
  return $form;
 }
