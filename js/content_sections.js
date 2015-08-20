(function( $ ) {
	$.fn.contentSectionEditor = function(action) {
		var $editors;
		if(this.filter('.content-sections-editor').length){
			$editors = this.filter('.content-sections-editor');
		}else{
			$editors = $('.content-sections-editor',this);
		}
		if(window.console){
			console.log('Editors :', $editors);
		}
		$editors.each(function(){
			if ( $.fn.contentSectionEditor.method[action] ) {
				return $.fn.contentSectionEditor.method[action].apply( this, Array.prototype.slice.call( arguments, 1 ));
			}else{
				return $.fn.contentSectionEditor.method.init.apply( this, Array.prototype.slice.call( arguments, 1 ));
			}
		});
		return $editors;
	};
	$.fn.contentSectionEditor.method = {};
	$.fn.contentSectionEditor.method.init = function(){
		var $editor = $(this);
		$editor.bind('SectionFormOpen',function(){
			$('.add-form-row .submitAddSection',$editor).show();
		});
		$('.add-form-row',$editor).hide();
		$('.new-row-row',$editor).show();
		$('.addSection',$editor).click(function(){
			$editor.contentSectionEditor('new');
			return false;
		});
		$('.cancelSection',$editor).click(function(){
			$editor.contentSectionEditor('cancel');
			return false;
		});
	}
	$.fn.contentSectionEditor.method['new'] = function(){
		var $editor = $(this);
		$('.new-row-row',$editor).after($('.add-form-row',$editor)).hide();
		$('.add-form-row',$editor).show();
		$('.add-form-container',$editor).empty();
		$('.add-form-row .plugin-selector',$editor).val('');
		$('.add-form-row .submitAddSection',$editor).hide();
		$('.add-form-row .label-field',$editor).val('');
	}
	$.fn.contentSectionEditor.method.cancel = function(){
		var $editor = $(this);
		$('.add-form-row',$editor).hide();
		$('.new-row-row',$editor).show();
	}	
	Drupal.behaviors.contentSectionEditor = {
		attach: function (context, settings) {
			if(window.console){
				console.log(context, settings);
			}
			$('.content-sections-editor',context).once('contentSectionEditor', function () {
				$(this).contentSectionEditor();
			});
		}
	};
})( jQuery );