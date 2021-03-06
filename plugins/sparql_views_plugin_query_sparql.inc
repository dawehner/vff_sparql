<?php
// $Id$

class sparql_views_plugin_query_sparql extends views_plugin_query {

  /**
   * Constructor; Create the basic query object and fill with default values.
   */
  function init($base_table = 'sparql_ep', $base_field, $query_options) {
    // @todo Add JS for hiding irrelvant field items.
    
    $this->endpoint_url = $query_options['endpoint_url'];
    $this->api_method = variable_get('sparql_views_api_method', 'json');
  }

  /**
   * Construct the "WHERE" or "HAVING" part of the query.
   *
   * @param $where
   *   'where' or 'having'.
   */
  function condition_query($where = 'where') {
    $clauses = array();
    if ($this->$where) {
      foreach ($this->$where as $group => $info) {
        $clause = implode($info['type'], $info['clauses']);
        if (count($info['clauses']) > 1) {
          $clause = $clause;
        }
        $clauses[] = $clause;
      }
      
      // Unset the empty clause that is added (apparently by default).
      unset($clauses[0]);
      if ($clauses) {
        if (count($clauses) > 1) {
          return implode(")" . $this->group_operator , $clauses);
        }
        else {
          return array_shift($clauses);
        }
      }
    }
    return "";
  }

  function use_pager() {
    return FALSE;
  }

  /**
   * Generate a query and a countquery from all of the information supplied
   * to the object.
   *
   * @param $get_count
   *   Provide a countquery if this is true, otherwise provide a normal query.
   */
  function query($view, $get_count = FALSE) {

    $where = $this->condition_query();

    $params = $this->params;
    if (!empty($view->pager['items_per_page'])) {
      $params['rpp'] = $view->pager['items_per_page'] <= 100 ? $view->pager['items_per_page'] : 100;
    }
    $params['WHERE'] = $where;

    // Build the full string.
    $items = array();
    if ($params) {
      foreach ($params as $key => $value) {
        if ($key == 'q') {
          array_unshift($items, 'query='. urlencode($value));
        }
        else if ($key == 'WHERE') {
          $where_condition = "\{". $value ."\}";
          $items[] = $key . urlencode($where_condition);
        }
        else {
          $items[] = $key .'='. urlencode($value);
        }
      }
    }
    $params = implode('&', $items);

    return $params;
  }


  /**
   * Get the arguments attached to the WHERE and HAVING clauses of this query.
   */
  function get_where_args() {
    $args = array();
    foreach ($this->where as $group => $where) {
      $args = array_merge($args, $where['args']);
    }

    if ($this->having) {
      foreach ($this->having as $group => $having) {
        $args = array_merge($args, $having['args']);
      }
    }
    return $args;
  }

  /**
   * Create a new grouping for the WHERE or HAVING clause.
   *
   * @param $type
   *   Either 'AND' or 'OR'. All items within this group will be added
   *   to the WHERE clause with this logical operator.
   * @param $group
   *   An ID to use for this group. If unspecified, an ID will be generated.
   * @param $where
   *   'where' or 'having'.
   *
   * @return $group
   *   The group ID generated.
   */
  function set_where_group($type = 'AND', $group = NULL, $where = 'where') {
    // Set an alias.
    $groups = &$this->$where;

    if (!isset($group)) {
      $group = empty($groups) ? 1 : max(array_keys($groups)) + 1;
    }

    if ($type === 'AND') {
      $type = ' ';
    }

    // Create an empty group
    if (empty($groups[$group])) {
      $groups[$group] = array('clauses' => array(), 'args' => array());
    }

    $groups[$group]['type'] = strtoupper($type);
    return $group;
  }

  function add_param($param, $value = '') {
    $this->params[$param] = $value;
  }

  function add_where($group, $clause) {
    $args = func_get_args();
    array_shift($args); // ditch $group
    array_shift($args); // ditch $clause

    // Expand an array of args if it came in.
    if (count($args) == 1 && is_array(reset($args))) {
      $args = current($args);
    }

    // Ensure all variants of 0 are actually 0. Thus '', 0 and NULL are all
    // the default group.
    if (empty($group)) {
      $group = 0;
    }

    // Check for a group.
    if (!isset($this->where[$group])) {
      $this->set_where_group('AND', $group);
    }

    // Add the clause and the args.
    if (is_array($args)) {
      $this->where[$group]['clauses'][] = $clause;
      // we use array_values() here to prevent array_merge errors as keys from multiple
      // sources occasionally collide.
      $this->where[$group]['args'] = array_merge($this->where[$group]['args'], array_values($args));
    }
  }

  /**
   * Let modules modify the query just prior to finalizing it.
   */
  function alter(&$view) {
    foreach (module_implements('sparql_views_query_alter') as $module) {
      $function = $module . '_sparql_views_query_alter';
      $function($view, $this);
    }
  }

  /**
   * Builds the necessary info to execute the query.
   */
  function build(&$view) {
    $this->init_pager($view);
    if ($this->pager->use_pager()) {
      $this->pager->set_current_page($view->current_page);
    }

    $view->build_info['query'] = $this->query($view);
    $view->build_info['count_query'] = $this->query($view, TRUE);
    $view->build_info['query_args'] = $this->get_where_args();
  }

  /**
   * Executes the query and fills the associated view object with according
   * values.
   *
   * Values to set: $view->result, $view->total_rows, $view->execute_time,
   * $view->pager['current_page'].
   */
  function execute(&$view) {
    $query = $view->build_info['query'];
    global $user;
    profile_load_profile($user);
    
    if ($query) {
      $replacements = module_invoke_all('views_query_substitutions', $view);
      $query = str_replace(array_keys($replacements), $replacements, $query);
    //$query = 'query=PREFIX+rdf:+%3Chttp://www.w3.org/1999/02/22-rdf-syntax-ns%23%3E+%0D%0APREFIX+rdfs:+%3Chttp://www.w3.org/2000/01/rdf-schema%23%3E+%0D%0APREFIX+dcterms:+%3Chttp://purl.org/dc/terms/%3E+%0D%0APREFIX+foaf:+%3Chttp://xmlns.com/foaf/0.1/%3E+%0D%0APREFIX+mpp:+%3Chttp://imp.deri.ie/ontology/moviePostProcessing%23%3E+%0D%0A%0D%0ASELECT+*+%0D%0AFROM+NAMED+%3Chttp://imp.deri.ie/vff/ppa/projects%3E+%0D%0AFROM+NAMED+%3Chttp://imp.deri.ie/vff/ppa/people%3E+%0D%0AWHERE+{%0D%0A%0D%0A%3C' . urlencode($user->profile_uri) .'%3E+foaf:currentProject+%3Fproject+.%0D%0A%3Fproject+rdfs:label+%3Ftitle+.%0D%0A%3Fperson+rdf:type+mpp:Person+;%0D%0A++++++++++rdfs:label+%3FpersonName+;%0D%0A++++++++++foaf:currentProject+%3Fproject+.%0D%0A}&output=json&jsonp=&key=imp';
    $query = 'query=PREFIX+rdf:+%3Chttp://www.w3.org/1999/02/22-rdf-syntax-ns%23%3E+%0D%0APREFIX+rdfs:+%3Chttp://www.w3.org/2000/01/rdf-schema%23%3E+%0D%0APREFIX+dcterms:+%3Chttp://purl.org/dc/terms/%3E+%0D%0APREFIX+foaf:+%3Chttp://xmlns.com/foaf/0.1/%3E+%0D%0APREFIX+mpp:+%3Chttp://imp.deri.ie/ontology/moviePostProcessing%23%3E+%0D%0A%0D%0ASELECT+*+%0D%0AFROM+NAMED+%3Chttp://imp.deri.ie/vff/ppa/projects%3E+%0D%0AFROM+NAMED+%3Chttp://imp.deri.ie/vff/ppa/people%3E+%0D%0A' . $query . '&output=json&jsonp=&key=imp';

      $url = $this->endpoint_url .'?'. $query;

      $start = views_microtime();
      
      $headers = array('Accept' => 'application/json');

      $results = drupal_http_request($url, $headers, 'GET');
      /*if (_twitter_request_failure($results)) {
        return FALSE;
      }*/

      switch ($this->api_method) {
        case 'atom':
          // Not supported yet.
          return;
        case 'json':
        default:
          $results = json_decode($results->data, TRUE);
          break;
      }

      if ($results['results']) {
        foreach ($results['results']['bindings'] as $resource) {
          $project = $resource['project']['value'];
          
          $view->result[$project]->project_title = array(array($resource['project']['value']=>$resource['title']['value']));
          $view->result[$project]->people[] = array($resource['person']['value']=>$resource['personName']['value']);
        }
        // Save the metadata into the object
        unset($results['results']);
        foreach ($results as $key => $value) {
          $this->$key = $value;
        }

        $this->execute_time = $this->completed_in;

        // FIXME
        //$this->total_rows = $this->results_per_page * 2;
        //$this->pager['current_page'] = $this->page -1;
      }
    }

    $view->execute_time = views_microtime() - $start;
  }

  function add_signature(&$view) {
    //$view->query->add_field(NULL, "'" . $view->name . ':' . $view->current_display . "'", 'view_name');
  }

  // This can only be displayed in the UI if the patch in
  // http://drupal.org/node/621142 is applied.
  function option_definition() {
    $options = parent::option_definition();
    $options['endpoint_url'] = array('default' => 'http://lab.linkeddata.deri.ie/ppa/sparql.php');

    return $options;
  }
  
  function admin_summary() {
    dpm($this);
    if (!empty($this->options['exposed'])) {
    }
  }

  function options_form(&$form, &$form_state) {
    $form['endpoint_url'] = array(
      '#type' => 'textfield',
      '#title' => t('SPARQL Endpoint'),
      '#default_value' => $this->endpoint_url,
      '#description' => t("The SPARQL endpoint that you want to access. For instance, !dbpedia.",
      array('!dbpedia' => l('http://dbpedia.org/sparql', 'http://dbpedia.org/sparql'))),
    );
  }

  function options_validate(&$form, &$form_state) {
  }

  function options_submit(&$form, &$form_state) {
  }
}

