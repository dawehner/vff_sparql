<?php
// $Id: vff_sparql.views.inc,v 1.3 2009/11/01 11:47:23 dereine Exp $

/**
 * Twitter API views search.
 */

function vff_sparql_views_data() {
  $data['sparql_ep']['table']['group']  = t('SPARQL endpoint');

  $data['sparql_ep']['table']['base'] = array(
    'title' => t('VFF'),
    'help' => t('Connects to http://lab.linkeddata.deri.ie/ppa/sparql.php to create Virtual Film Factory front end.'),
    'query class' => 'sparql_query'
  );

  // String
  $data['sparql_ep']['string'] = array(
    'title' => t('String'),
    'help' => t('Search for a string on twitter'),
    'field' => array(
      'handler' => 'vff_sparql_handler_field',
    ),
    'filter' => array(
      'handler' => 'vff_sparql_handler_filter_string',
    ),
  );

  $data['sparql_ep']['project_title'] = array(
    'title' => t('Project Title'),
    'help' => t('Display the text of a twitter message'),
    // Shouldn't there be vff_sparql_handler_field_markup??
    'field' => array(
      'handler' => 'vff_sparql_handler_field_link',
    ),
  );
  
  $data['sparql_ep']['people'] = array(
    'title' => t('People on a project'),
    'help' => t('Display the text of a twitter message'),
    // Shouldn't there be vff_sparql_handler_field_markup??
    'field' => array(
      'handler' => 'vff_sparql_handler_field_link',
    ),
  );
  
  $data['sparql_ep']['source'] = array(
    'title' => t('Source'),
    'help' => t('Display the source of a tweet'),
    'field' => array(
      'handler' => 'vff_sparql_handler_field_source',
    ),
  );

  $data['sparql_ep']['from_user'] = array(
    'title' => t('Tweet Author'),
    'help' => t('Display the author of the tweet'),
    'field' => array(
      'handler' => 'vff_sparql_handler_field_from_user',
     ),
     'filter' => array(
      'handler' => 'vff_sparql_handler_filter_from_user',
     ),
  );



  $data['sparql_ep']['reference'] = array(
    'title' => t('Tweet reference'),
    'help' => t('Display the  referencing person  of the tweet'),
    'field' => array(
      'handler' => 'vff_sparql_handler_field_reference',
     ),
     'filter' => array(
      'handler' => 'vff_sparql_handler_filter_reference',
     ),
  );


  $data['sparql_ep']['hash'] = array(
    'title' => t('Tweet hash'),
    'field' => array(
      'handler' => 'vff_sparql_handler_field_hash',
     ),
     'filter' => array(
      'handler' => 'vff_sparql_handler_filter_hash',
     ),
  );

  $data['sparql_ep']['attitude'] = array(
    'title' => t('Tweet attitude'),
    'help' => t('Did the tweet contained happieness, sadness or a question'),
    'filter' => array(
      'handler' => 'vff_sparql_handler_filter_attitude',
    ),
  );

  $data['sparql_ep']['locale'] = array(
    'title' => t('Tweet locale'),
    'filter' => array(
      'handler' => 'vff_sparql_handler_filter_locale',
    ),
  );

  return $data;
}

/**
 * Implementation of hook_views_handlers().
 */
function vff_sparql_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'vff_sparql') . '/handlers',
    ),
    'handlers' => array(
      // Fields
      'vff_sparql_handler_field' => array(
        'parent' => 'views_handler_field'
      ),
      'vff_sparql_handler_field_source' => array(
        'parent' => 'vff_sparql_handler_field',
      ),
      'vff_sparql_handler_field_link' => array(
        'parent' => 'vff_sparql_handler_field',
      ),
      'vff_sparql_handler_field_markup' => array(
        'parent' => 'vff_sparql_handler_field',
      ),
      'vff_sparql_handler_field_from_user' => array(
        'parent' => 'vff_sparql_handler_field'
      ),

      // Filters
      'vff_sparql_handler_filter' => array(
        'parent' => 'views_handler_filter'
      ),
      'vff_sparql_handler_filter_string' => array(
        'parent' => 'vff_sparql_handler_filter'
      ),
      'vff_sparql_handler_filter_from_user' => array(
        'parent' => 'vff_sparql_handler_filter'
      ),
      'vff_sparql_handler_filter_hash' => array(
        'parent' => 'vff_sparql_handler_filter'
      ),
      'vff_sparql_handler_filter_attitude' => array(
        'parent' => 'vff_sparql_handler_filter'
      ),
      'vff_sparql_handler_filter_locale' => array(
        'parent' => 'vff_sparql_handler_filter'
      ),
    ),
  );
}

/**
 * Implementation of hook_views_plugins().
 */
function vff_sparql_views_plugins() {
  return array(
    'query' => array(
      'sparql_query' => array(
        'title' => t('SPARQL Query'),
        'help' => t('Query will be generated and run against a SPARQL endpoint.'),
        'handler' => 'vff_sparql_plugin_query_sparql'
      ),
    ),
  );
}
