<?php
//not currently used.
class CalsExternalSearchController extends DrupalDefaultEntityController { 

  public function save($external_search) {
    drupal_write_record('external_search', $external_search);
    field_attach_insert('external_search', $external_search);
    module_invoke_all('entity_insert', 'external_search', $external_search);
    return $external_search;
  }
}