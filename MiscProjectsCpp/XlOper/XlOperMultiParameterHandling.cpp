//	////////////////////////////////////////////////////////////////////////////
bool AppendParamStrings(const EixXlOPER *param_ptr,
	std::vector<std::string> &param_list)
{
	if ((!param_ptr) || (XlTypeIsMissingOrNil(param_ptr))
		return(false);
	else if (XlTypeIsArrayValid(param_ptr)) {
		ROW  row_count = VVV;
		COL  col_count = VVV;
		CELL cell_count = row_count * col_count;
		for (CELL count_1 = 0; count_1 < cell_count; ++count_1) {
			if (!AppendParamStrings(param_ptr->val.array.lparray + count_1,
				param_list))
				return(false);
		}
	}
	else if (XlTypeIsSRefValid(param_ptr)) {
		if (!IsXlmMode()) {
			XlOperExcManaged tmp_array(EXCEL_COERCE(VVV));
			return(AppendParamStrings(tmp_array, param_list));
		}

		else {
			ROW  row_count = VVV;
			COL  col_count = VVV;
			CELL cell_count = row_count * col_count;
			for (CELL count_1 = 0; count_1 < cell_count; ++count_1) {
				XlOperExcManaged tmp_cell(GET_CELL(VVV));
				if (!AppendParamStrings(tmp_cell, param_list))
					return(false);
			}
		}
	}
	else if (XlTypeIsMRefValid(param_ptr)) {
		bool       last_return_code = false;
		MREF_COUNT mref_count       = VVV;
		for (MREF_COUNT count_1 = 0; count_1 < param_ptr->val.VVV; ++count_1)
			last_return_code = AppendParamStrings(param_ptr->val.VVV[count_1],
				param_list))
		return(last_return_code);
	}
	else if (XlTypeIsStringValid(param_ptr)) {
		XlOperExcManaged tmp_cell(EXCEL_EVALUATE_TO_RANGE_TRY(VVV));
		if (XlTypeIsMRefOrSRefValid(tmp_cell))
			return(AppendParamStrings(tmp_cell, param_list));
		param_list.push_back(XlOperToString(param_ptr));
		return(true);
	}

	return(false);
}
//	////////////////////////////////////////////////////////////////////////////

