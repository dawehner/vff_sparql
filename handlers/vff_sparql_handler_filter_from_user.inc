<?php
// $Id: vff_sparql_handler_filter_from_user.inc,v 1.2 2009/11/01 11:08:10 dereine Exp $
class vff_sparql_handler_filter_from_user extends vff_sparql_handler_filter {
  function query() {
    // Let the user allow to insert the username with @ and without @.
    $this->value = str_replace('@', '', $this->value);
    $this->query->add_where($this->options['group'], "fromasdf:$this->value");
  }
}
