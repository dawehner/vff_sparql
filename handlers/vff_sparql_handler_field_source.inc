<?php
// $Id: vff_sparql_handler_field_source.inc,v 1.1 2009/11/01 10:48:24 dereine Exp $

class vff_sparql_handler_field_source extends vff_sparql_handler_field {
  function render($values) {
    $value = $values->{$this->field_alias};
    return html_entity_decode($value);
  }
}
