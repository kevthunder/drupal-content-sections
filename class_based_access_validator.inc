<?php
	
class ClassBasedAccessValidator {
	var $validators = array(
		'/^role-(.*)$/' => array('ClassBasedAccessValidator','check_role'),
		'/^not-role-(.*)$/' => array('ClassBasedAccessValidator','check_not_role'),
		'/^permission-(.*)$/' => array('ClassBasedAccessValidator','check_permission'),
		'/^(not-)?mobile$/' => array('ClassBasedAccessValidator','check_mobile'),
		'/^feedback-(not-)?filled$/' => array('ClassBasedAccessValidator','check_feedback_filled'),
		'/^newsletter-(not-)?subscripted$/' => array('ClassBasedAccessValidator','check_newsletter_subscripted'),
	);

	function addValidator($pattern,$funct){
		$this->validators[$pattern] = $funct;
	}
	
	function check($classes,$account){
		if(!empty($classes)){
			if(!is_array($classes)){
				$classes = explode(' ',$classes);
			}
			foreach($classes as $class){
				foreach($this->validators as $pattern => $funct){
					if(preg_match($pattern, $class, $matches)){
						//kpr(compact('class','account'));
						$res = call_user_func_array($funct,array($matches,$account));
						if($res === false) return false;
					}
				}
			}
		}
		return true;
	}
	function check_role($matches,$account){
		if(!in_array(str_replace('-',' ',$matches[1]),$account->roles)){
			return false;
		}
	}
	
	function check_not_role($matches,$account){
		if(in_array(str_replace('-',' ',$matches[1]),$account->roles)){
			return false;
		}
	}
	
	function check_permission($matches,$account){
		if(!user_access($matches[1], $account) && !user_access(str_replace('-',' ',$matches[1]), $account) && !user_access(str_replace('-','_',$matches[1]), $account)){
			return false;
		}
	}
	
	function check_mobile($matches,$account){
		if(module_exists('mobile_theme')){
			return empty($matches[1]) == (bool)mobile_theme_is_mobile();
		}
		return true;
	}
	function check_feedback_filled($matches,$account){

    $feedback_popup_nid = variable_get('abbott_custom_feedback_popup_nid');
    $enabled = webform_is_enabled(abbott_custom_get_translated_nid($feedback_popup_nid));
    
    // kpr(array($feedback_popup_nid,$enabled));
    
		return empty($matches[1]) != (bool)$enabled;

		return true;
	}
	function check_newsletter_subscripted($matches,$account){
    $trial = abbott_patients_my_current_trial();
    if( !empty($trial->trial->field_newsletter_trial['und'][0]['value']) || !empty($trial->trial->field_newsletter_milestone['und'][0]['value'])){
      $subscripted = !empty($trial->field_newsletter_trial['und'][0]['value']) && !empty($trial->field_newsletter_milestone['und'][0]['value']);
      // kpr($trial);
      
      return empty($matches[1]) == (bool)$subscripted;
    }
		return false;
	}
	
}
?>