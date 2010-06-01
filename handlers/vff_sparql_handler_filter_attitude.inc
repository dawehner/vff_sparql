<?php
// $Id: vff_sparql_handler_filter_attitude.inc,v 1.1 2009/11/01 11:33:13 dereine Exp $
/**
 * @file
 *   Filter by tweets by attitude.
 */

define('vff_sparql_ATTITUDE_SAD', ':(');
define('vff_sparql_ATTITUDE_HAPPY', ':)');
define('vff_sparql_ATTITUDE_QUESTION', '?');

class vff_sparql_handler_filter_attitude extends vff_sparql_handler_filter {
  function option_definition() {
    $options = parent::option_definition();

    return $options;
  }

  function admin_summary() {
    if (!empty($this->options['exposed'])) {
      return t('exposed');
    }

    $output = $this->options['value'];

    return $output;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['value'] = array(
      '#type' => 'select',
      '#title' => t('Tweet attitude'),
      '#options' => array(
        '' => t('None'),
        vff_sparql_ATTITUDE_SAD => t('Sadness'),
        vff_sparql_ATTITUDE_HAPPY => t('Happyness'),
        vff_sparql_ATTITUDE_QUESTION => t('Question'),
      ),
      '#default_value' => $this->options['value'],
    );
  }

  function query() {
    if ($this->value) {
      $this->query->add_where($this->options['group'], "$this->value");
    }
  }
}
