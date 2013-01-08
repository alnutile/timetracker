<?php


function timetracker_admin_report(){
  $output = print_r($arg, 1);
  
  return $output;
}



/** 
 * the import form
*/

function timetracker_admin_uploader($form_state) {
  $form = array();
  $form['upload'] = array(
    '#type' => 'file',
    '#title' => t('Upload CSV file'),
    '#size' => 22,
    '#weight' => -10,
  );
  
  if(isset($form_state['values']['all'])) {
    $form['output'] = array('#markup' => $form_state['values']['all']);
  }
  
  $form['submit'] = array(
			  '#type' => 'submit',
			  '#value' => 'Submit',
			  );
  return $form;
}

function timetracker_admin_uploader_validate($form, &$form_state) {
  $file = file_save_upload('upload', array(
    'file_validate_extensions' => array('csv'), // Validate extensions.
  ));

  // If the file passed validation:
  if ($file) {
    // Move the file, into the Drupal file system
    if ($file = file_move($file, 'public://')) {
      // Save the file for use in the submit handler.
      $form_state['storage']['file'] = $file;
      
    }
    else {
      form_set_error('file', t('Failed to write the uploaded file the site\'s file folder.'));
    }
  }
  else {
    form_set_error('file', t('No file was uploaded.'));
  }
}

function timetracker_admin_uploader_submit($form, &$form_state) {
  dpm($form_state['storage']['file']);
  $file = $form_state['storage']['file'];
  unset($form_state['storage']['file']);
  //Time to read and import the file
  $importFile = timetracker_import_file($file);
  dpm($importFile);
  drupal_set_message(t('The file has been uploaded,
		       filename: @filename.', array('@filename' => $file->filename)));
  $form_state['values']['all'] = $importFile;
  $form_state['rebuild'] = TRUE;
}


function timetracker_import_file($file) {
  $path = file_create_url($file->uri);
  $get = fopen($path, "r");
  $count = 0;
  while($data = fgetcsv($get, 1000, ",")){
    //Skip first row
    if($count > 0) {
      list($group, $date, $task, $hours, $earnings, $notes) = $data;
      dpm("$group $date  $task  $hours $earnings $notes");
      $time_node = timetracker_node_add_it($group, $date, $task, $hours, $earnings, $notes);
      dpm($time_node);
      $count++;
    } else {
      $headers = $data;
      $count++;
    }
  }
  //return theme('item_list', array('items' => $rows));
}

function timetracker_node_add_it($group = NULL, $date,  $task,  $hours, $earnings = NULL, $notes = NULL) {
  global $user;
  $node = new stdClass();
  $node->type = 'times';
	node_object_prepare($node);
	$node->uid = $user->uid;
	$node->name = $user->name;
	
	$node->title = check_plain($task);
	$node->language = LANGUAGE_NONE;
	//@todo not really using the body? 
	//  This will not work as is
	$node->body[$node->language][0]['value'] = $notes;
	$node->body[$node->language][0]['summary'] = text_summary($notes);
	$node->body[$node->language][0]['format'] = 'plain_text';
	
	$unix = date('U', strtotime($hours));
	$node->field_date['und'][0]['value'] = check_plain($unix);
	$node->field_time['und'][0]['value'] = check_plain($hours);
	$node->field_billable['und'][0]['value'] = check_plain($group);
	
	$node->status = 1;
	$node->promote = 0;
	$node->revision = $user->uid;
	
	$node->changed = $_SERVER['REQUEST_TIME'];
	$node->created = $_SERVER['REQUEST_TIME'];
	
	node_submit($node);
	$nodeMade = node_save($node);
	return $nodeMade;
}