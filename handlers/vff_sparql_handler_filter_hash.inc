<?php
// $ID$
class vff_sparql_handler_filter_hash extends vff_sparql_handler_filter {
  function query() {
    // Let the user allow to insert the hash with # and without #.
    $this->value = str_replace('#', '', $this->value);
    // Twitter does only allow to search for hash with the length 16.
    $this->value = drupal_substr($this->value, 0, 16);
    $this->query->add_where($this->options['group'], "#$this->value");
  }
}
