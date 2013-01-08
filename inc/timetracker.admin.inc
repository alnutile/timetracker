<?php


function timetracker_admin_report(){
  $output = print_r($arg, 1);
  
  return $output;
}



/** 
 * the import form
*/

function timetracker_admin_uploader($form, &$form_state) {
  $form = array();
  $form['upload'] = array(
    '#type' => 'file',
    '#title' => t('Upload CSV file'),
    '#size' => 22,
    '#theme_wrappers' => array(),
    '#weight' => -10,
  );
  
  $form['submit'] = array(
			  '#type' => 'submit',
			  '#value' => 'Submit',
			  );
  return $form;
}

function timetracker_admin_uploader_validate($form, &$form_state) {
  $path = drupal_get_path('module', 'timetracker');
  $file = file_save_upload('upload');
  if ($file) {
      file_unmanaged_move('temporary://' . $file->filename, 'public://', $replace = FILE_EXISTS_REPLACE);
      $form_state['values']['upload'] = $file;
  }
  else {
    form_set_error('file', t('Things are not right in the file world...'));
  }
}

function timetracker_admin_uploader_submit($form, &$form_state) {
  $file =$form_state['values']['upload'];
  unset($form_state['values']['upload']);
  drupal_set_message(t('The file has been uploaded,
		       filename: @filename.', array('@filename' => $file->filename)));
}
